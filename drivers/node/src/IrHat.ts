import { Observable, Subject, Subscription } from "rxjs";
import {
  RawIrHat,
  RawIrHatMessageSignal,
  RawIrHatOptions,
  RawIrHatSignal,
} from "./RawIrHat";
import Debug from "debug";

const debug = Debug("irhat:IrHat");

export class IrHat {
  private rxSubject = new Subject<IrHatMessage>();
  private rawIrHat: RawIrHat;
  private rawIrHatRxSubscription?: Subscription;

  constructor(options: IrHatOptions | RawIrHatIrHatOptions) {
    if (isRawIrHatIrHatOptions(options)) {
      this.rawIrHat = (options as RawIrHatIrHatOptions).rawIrHat;
    } else {
      this.rawIrHat = new RawIrHat(options);
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

  sendSignal(signal: IrHatSignal): Promise<void> {
    return this.sendAndWaitForResponse(() => {
      return this.rawIrHat.sendSignal(signal);
    });
  }

  private async sendAndWaitForResponse(
    sendFn: () => Promise<void>
  ): Promise<void> {
    // TODO begin listening for response
    await sendFn();
    // TODO wait for response
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
