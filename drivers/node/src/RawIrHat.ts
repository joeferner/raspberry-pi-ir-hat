import { Observable, Subject, Subscription } from "rxjs";
import {
  IrHatSerialPort,
  IrHatSerialPortImpl,
  IrHatSerialPortOptions,
} from "./IrHatSerialPort";
import { Protocol } from "./Protocol";
import Debug from "debug";
import { IrHatGpio, IrHatGpioImpl } from "./IrHatGpio";
import { sleep } from "./utils";

const debug = Debug("irhat:RawIrHat");

export interface RawIrHat {
  open(): Promise<void>;
  close(): Promise<void>;
  sendSignal(signal: RawIrHatSignal): Promise<void>;
  isOpen(): boolean;
  reset(): Promise<void>;
  get rx(): Observable<RawIrHatMessage>;
}
export class RawIrHatImpl implements RawIrHat {
  private rxSubject = new Subject<RawIrHatMessage>();
  private serialPortRxSubscription?: Subscription;
  private serialPort: IrHatSerialPort;
  private resetGpio: IrHatGpio;
  private buffer = "";
  private resetDuration: number;

  constructor(options: IocRawIrHatOptions | RawIrHatOptions) {
    if (isIocRawIrHatOptions(options)) {
      this.serialPort = (options as IocRawIrHatOptions).serialPort;
      this.resetGpio = (options as IocRawIrHatOptions).resetGpio;
      this.resetDuration = (options as IocRawIrHatOptions).resetDuration;
    } else {
      this.serialPort = new IrHatSerialPortImpl(options);
      this.resetGpio = new IrHatGpioImpl();
      this.resetDuration = 500;
    }
  }

  async open(): Promise<void> {
    if (this.isOpen()) {
      throw new Error("raw hat already open");
    }
    debug("opening RawIrHat");
    await this.serialPort.open();
    this.buffer = "";
    this.serialPortRxSubscription = this.serialPort.rx.subscribe((next) => {
      this.buffer += next;
      this.processBuffer();
    });
    await this.reset();
  }

  close(): Promise<void> {
    if (!this.isOpen()) {
      throw new Error("raw hat is not open");
    }
    debug("closing RawIrHat");
    this.serialPortRxSubscription?.unsubscribe();
    return this.serialPort.close();
  }

  sendSignal(signal: RawIrHatSignal): Promise<void> {
    if (!this.isOpen()) {
      throw new Error("raw hat is not open");
    }
    let data = `+s${signal.protocol.toFixed(0)}`;
    data += `,${signal.address.toFixed(0)}`;
    data += `,${signal.command.toFixed(0)}`;
    if (signal.numberOfRepeats !== undefined) {
      data += `,${signal.numberOfRepeats.toFixed(0)}`;
    }
    data += "\n";
    return this.serialPort.write(data);
  }

  isOpen(): boolean {
    return this.serialPort.isOpen() && !!this.serialPortRxSubscription;
  }

  async reset(): Promise<void> {
    debug("reset");
    await this.resetGpio.write(0);
    await sleep(this.resetDuration);
    await this.resetGpio.write(1);
  }

  private processBuffer(): void {
    while (true) {
      let i = this.buffer.indexOf("\n");
      if (i < 0) {
        break;
      }
      const line = this.buffer.substring(0, i).trim();
      if (line.length > 0) {
        this.processLine(line);
      }
      this.buffer = this.buffer.substring(i + 1);
    }
  }

  private processLine(line: string): void {
    debug("processLine: %s", line);
    if (line.startsWith("!s")) {
      this.processSignalLine(line.substring("!s".length));
    } else if (line.startsWith("?READY")) {
      this.rxSubject.next({
        type: "ready",
      });
    } else if (line.startsWith("+OK")) {
      this.rxSubject.next({
        type: "ok",
        message: line.substring("+OK".length).trim(),
      });
    } else if (line.startsWith("-ERR")) {
      this.rxSubject.next({
        type: "error",
        error: line.substring("-ERR".length).trim(),
      });
    } else {
      this.rxSubject.next({
        type: "unknownLine",
        line,
      });
    }
  }

  private processSignalLine(line: string): void {
    const parts = line.split(",");
    if (parts.length !== 7) {
      return this.rxSubject.next({
        type: "error",
        error: `expected 7 parts to signal line, found ${parts.length}: ${line}`,
      });
    }

    const protocol = parseInt(parts[0], 10);
    const address = parseInt(parts[1], 10);
    const command = parseInt(parts[2], 10);
    const repeat = parseInt(parts[3], 10);
    const autoRepeat = parseInt(parts[4], 10);
    const parityFailed = parseInt(parts[5], 10);
    const delta = parseInt(parts[6], 10);

    if (isNaN(protocol)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid protocol: ${line}`,
      });
    }
    if (isNaN(address)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid address: ${line}`,
      });
    }
    if (isNaN(command)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid command: ${line}`,
      });
    }
    if (isNaN(repeat)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid repeat: ${line}`,
      });
    }
    if (isNaN(autoRepeat)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid autoRepeat: ${line}`,
      });
    }
    if (isNaN(parityFailed)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid parityFailed: ${line}`,
      });
    }
    if (isNaN(delta)) {
      return this.rxSubject.next({
        type: "error",
        error: `invalid delta: ${line}`,
      });
    }

    this.rxSubject.next({
      type: "signal",
      protocol: protocol as Protocol,
      address,
      command,
      repeat,
      autoRepeat,
      parityFailed,
      delta,
    });
  }

  get rx(): Observable<RawIrHatMessage> {
    return this.rxSubject;
  }
}

function isIocRawIrHatOptions(
  x: IocRawIrHatOptions | RawIrHatOptions
): x is IocRawIrHatOptions {
  return !!(x as IocRawIrHatOptions).serialPort;
}

export interface IocRawIrHatOptions {
  serialPort: IrHatSerialPort;
  resetGpio: IrHatGpio;
  resetDuration: number;
}

export interface RawIrHatOptions extends IrHatSerialPortOptions {}

export interface RawIrHatMessageUnknownLine {
  type: "unknownLine";
  line: string;
}

export interface RawIrHatMessageReady {
  type: "ready";
}

export interface RawIrHatMessageOk {
  type: "ok";
  message: string;
}

export interface RawIrHatMessageError {
  type: "error";
  error: string;
}

export interface RawIrHatMessageSignal {
  type: "signal";
  protocol: Protocol;
  address: number;
  command: number;
  repeat: number;
  autoRepeat: number;
  parityFailed: number;
  delta: number;
}

export type RawIrHatMessage =
  | RawIrHatMessageUnknownLine
  | RawIrHatMessageReady
  | RawIrHatMessageError
  | RawIrHatMessageOk
  | RawIrHatMessageSignal;

export interface RawIrHatSignal {
  protocol: Protocol;
  address: number;
  command: number;
  numberOfRepeats?: number;
}
