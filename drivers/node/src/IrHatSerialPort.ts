import { Observable, Subject } from "rxjs";
import { SerialPort } from "serialport";

const BAUD_RATE = 57600;

export interface IrHatSerialPort {
  open(): Promise<void>;
  close(): Promise<void>;
  isOpen(): boolean;
  write(data: string): Promise<void>;
  get rx(): Observable<string>;
}

export class IrHatSerialPortImpl implements IrHatSerialPort {
  private readonly port: SerialPort;
  private readonly rxSubject = new Subject<string>();

  constructor(options: IrHatSerialPortOptions) {
    this.port = new SerialPort({
      path: options.path,
      baudRate: BAUD_RATE,
      autoOpen: false,
    });
    this.port.on("data", (chunk: any) => {
      this.rxSubject.next(chunk.toString("utf8"));
    });
  }

  isOpen(): boolean {
    return this.port.isOpen;
  }

  open(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      this.port.open((err) => {
        if (err) {
          return reject(err);
        }
        return resolve();
      });
    });
  }

  close(): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      this.port.close((err) => {
        if (err) {
          return reject(err);
        }
        return resolve();
      });
    });
  }

  write(data: string): Promise<void> {
    return new Promise<void>((resolve, reject) => {
      if (!this.port.write(data, "ascii")) {
        return reject(new Error("buffer overflow"));
      }
      this.port.drain((err) => {
        if (err) {
          return reject(err);
        }
        return resolve();
      });
    });
  }

  get rx(): Observable<string> {
    return this.rxSubject;
  }
}

export interface IrHatSerialPortOptions {
  path: string;
}
