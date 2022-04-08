import "jest";
import { Observable, Subject } from "rxjs";
import { Protocol } from ".";
import { IrHatSerialPort } from "./IrHatSerialPort";
import {
  RawIrHatImpl,
  RawIrHatMessage,
  RawIrHatMessageSignal,
} from "./RawIrHat";
import { sleep } from "./utils";

describe("RawIrHat", () => {
  let mockSerialPort: MockIrHatSerialPort;
  let rawIrHat: RawIrHatImpl;
  let rawIrHatMessages: RawIrHatMessage[];

  beforeEach(() => {
    mockSerialPort = new MockIrHatSerialPort();
    rawIrHatMessages = [];
    rawIrHat = new RawIrHatImpl({ serialPort: mockSerialPort });
    rawIrHat.rx.subscribe((d) => rawIrHatMessages.push(d));
  });

  describe("after open", () => {
    beforeEach(async () => {
      await rawIrHat.open();
    });
    afterEach(async () => {
      await rawIrHat.close();
    });

    it("send signal", async () => {
      await rawIrHat.sendSignal({
        protocol: Protocol.Denon,
        address: 1,
        command: 2,
      });
      expect(mockSerialPort.writeData).toStrictEqual(["+s1,1,2\n"]);
    });

    it("receive signal", async () => {
      mockSerialPort.rxSubject.next("!s1,2,3,4,5,6,7\n");
      await sleep(1);
      const msg: RawIrHatMessageSignal = {
        type: "signal",
        protocol: Protocol.Denon,
        address: 2,
        command: 3,
        repeat: 4,
        autoRepeat: 5,
        parityFailed: 6,
        delta: 7,
      };
      expect(rawIrHatMessages).toStrictEqual([msg]);
    });
  });
});

class MockIrHatSerialPort implements IrHatSerialPort {
  rxSubject = new Subject<string>();
  writeData: string[] = [];
  _open = false;

  open(): Promise<void> {
    this._open = true;
    return Promise.resolve();
  }

  close(): Promise<void> {
    this._open = false;
    return Promise.resolve();
  }

  isOpen(): boolean {
    return this._open;
  }

  async write(data: string): Promise<void> {
    this.writeData.push(data);
  }

  get rx(): Observable<string> {
    return this.rxSubject;
  }
}
