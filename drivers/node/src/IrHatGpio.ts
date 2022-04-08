export interface IrHatGpio {
  write(value: number): Promise<void>;
}

export class IrHatGpioImpl implements IrHatGpio {
  write(value: number): Promise<void> {
    throw new Error("Method not implemented.");
  }
}
