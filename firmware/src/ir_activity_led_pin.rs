use crate::hal::gpio::gpioa::PA7;
use crate::hal::gpio::OutputPin;

extern crate cortex_m_rt as rt;

pub struct IrActivityLedPin {
    pin: PA7,
}

impl IrActivityLedPin {
    pub fn new(pin: PA7) -> IrActivityLedPin {
        return IrActivityLedPin { pin };
    }

    pub fn toggle(&mut self) {
        return self.pin.toggle();
    }
}
