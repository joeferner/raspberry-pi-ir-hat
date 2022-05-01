import { BinaryValue, Gpio } from "onoff";

export interface IrHatGpio {
  write(value: BinaryValue): Promise<void>;
  close(): Promise<void>;
}

export class IrHatGpioImpl implements IrHatGpio {
  private pin: number;
  private gpio?: Gpio;

  constructor(pin: number) {
    this.pin = pin;
  }

  write(value: BinaryValue): Promise<void> {
    if (!this.gpio) {
      this.gpio = new Gpio(this.pin, "out");
    }
    return this.gpio.write(value);
  }

  async close(): Promise<void> {
    await this.gpio?.unexport();
    this.gpio = undefined;
  }
}
