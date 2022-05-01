import "jest";
import { Observable, Subject } from "rxjs";
import { IrHat, IrHatMessage } from "./IrHat";
import { Protocol } from "./Protocol";
import {
  RawIrHat,
  RawIrHatMessage,
  RawIrHatMessageSignal,
  RawIrHatSignal,
} from "./RawIrHat";
import { sleep } from "./utils";

describe("RawIrHat", () => {
  let mockRawIrHat: MockRawIrHat;
  let irHat: IrHat;

  beforeEach(() => {
    mockRawIrHat = new MockRawIrHat();
    irHat = new IrHat({ rawIrHat: mockRawIrHat });
  });

  describe("after open", () => {
    beforeEach(async () => {
      await irHat.open();
    });
    afterEach(async () => {
      await irHat.close();
    });

    it("send signal", async () => {
      const sendSignalPromise = irHat.sendSignal(
        {
          protocol: Protocol.Denon,
          address: 1,
          command: 2,
        },
        { timeout: 100 }
      );
      await sleep(10);
      mockRawIrHat.rxSubject.next({
        type: "ok",
        message: "",
      });
      await sendSignalPromise;
      expect(mockRawIrHat.sentSignals).toStrictEqual([
        {
          protocol: Protocol.Denon,
          address: 1,
          command: 2,
        },
      ]);
    });

    it("receive signal", async () => {
      const messages: IrHatMessage[] = [];
      irHat.rx.subscribe((msg) => {
        messages.push(msg);
      });

      const signal: RawIrHatMessageSignal = {
        type: "signal",
        protocol: Protocol.Denon,
        address: 1,
        command: 2,
        autoRepeat: 1,
        delta: 100,
        parityFailed: 0,
        repeat: 1,
      };
      mockRawIrHat.rxSubject.next(signal);
      await sleep(1);
      expect(messages).toStrictEqual([signal]);
    });

    it("reset", async () => {
      await irHat.reset();
      expect(mockRawIrHat.resetCalled).toBe(true);
    });
  });
});

class MockRawIrHat implements RawIrHat {
  private _open = false;
  resetCalled = false;
  readonly rxSubject = new Subject<RawIrHatMessage>();
  readonly sentSignals: RawIrHatSignal[] = [];

  open(): Promise<void> {
    if (this._open) {
      throw new Error("already open");
    }
    this._open = true;
    return Promise.resolve();
  }

  close(): Promise<void> {
    if (!this._open) {
      throw new Error("not open");
    }
    this._open = false;
    return Promise.resolve();
  }

  sendSignal(signal: RawIrHatSignal): Promise<void> {
    this.sentSignals.push(signal);
    return Promise.resolve();
  }

  isOpen(): boolean {
    return this._open;
  }

  reset(): Promise<void> {
    this.resetCalled = true;
    return Promise.resolve();
  }

  get rx(): Observable<RawIrHatMessage> {
    return this.rxSubject;
  }
}
