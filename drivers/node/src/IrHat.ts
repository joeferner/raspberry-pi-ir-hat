import { Observable, Subject, Subscription } from "rxjs";
import {
  RawIrHat,
  RawIrHatImpl,
  RawIrHatMessage,
  RawIrHatMessageSignal,
  RawIrHatOptions,
  RawIrHatSignal,
} from "./RawIrHat";
import Debug from "debug";
import { sleep } from "./utils";

const debug = Debug("irhat:IrHat");

export class IrHat {
  private rxSubject = new Subject<IrHatMessage>();
  private rawIrHat: RawIrHat;
  private rawIrHatRxSubscription?: Subscription;

  constructor(options: IrHatOptions | RawIrHatIrHatOptions) {
    if (isRawIrHatIrHatOptions(options)) {
      this.rawIrHat = (options as RawIrHatIrHatOptions).rawIrHat;
    } else {
      this.rawIrHat = new RawIrHatImpl(options);
    }
  }

  async open(): Promise<void> {
    if (this.rawIrHatRxSubscription) {
      throw new Error("hat already open");
    }
    debug("opening IrHat");
    await this.rawIrHat.open();
    this.rawIrHatRxSubscription = this.rawIrHat.rx.subscribe((rawMessage) => {
      debug("raw hat message: %o", rawMessage);
      switch (rawMessage.type) {
        case "signal":
          this.rxSubject.next(rawMessage);
          break;
      }
    });
  }

  close(): Promise<void> {
    debug("closing IrHat");
    this.rawIrHatRxSubscription?.unsubscribe();
    return this.rawIrHat.close();
  }

  sendSignal(signal: IrHatSignal, options?: SendSignalOptions): Promise<void> {
    debug("sendSignal %o", signal);
    return this.sendAndWaitForResponse(() => {
      return this.rawIrHat.sendSignal(signal);
    }, options);
  }

  private async sendAndWaitForResponse(
    sendFn: () => Promise<void>,
    options?: SendAndWaitOptions
  ): Promise<void> {
    const optionsWithDefaults = { timeout: 5000, ...options };
    const messageQueue: RawIrHatMessage[] = [];
    const subscription = this.rawIrHat.rx.subscribe((msg) => {
      messageQueue.push(msg);
    });
    try {
      // TODO do lock
      await sendFn();
      const endTime = Date.now() + optionsWithDefaults.timeout;
      while (Date.now() < endTime) {
        while (messageQueue.length > 0) {
          const message = messageQueue.splice(0, 1)[0];
          if (message.type === "ok") {
            return;
          } else if (message.type === "error") {
            throw new Error(message.error);
          }
        }
        await sleep(1);
      }
      throw new Error("timeout");
    } finally {
      subscription.unsubscribe();
    }
  }

  get rx(): Observable<IrHatMessage> {
    return this.rxSubject;
  }
}

function isRawIrHatIrHatOptions(
  x: IrHatOptions | RawIrHatIrHatOptions
): x is RawIrHatIrHatOptions {
  return !!(x as RawIrHatIrHatOptions).rawIrHat;
}

export interface RawIrHatIrHatOptions {
  rawIrHat: RawIrHat;
}

export interface IrHatOptions extends RawIrHatOptions {}

export interface IrHatMessageSignal extends RawIrHatMessageSignal {}

export type IrHatMessage = IrHatMessageSignal;

export interface IrHatSignal extends RawIrHatSignal {}

interface SendAndWaitOptions {
  timeout?: number;
}

export interface SendSignalOptions extends SendAndWaitOptions {}
