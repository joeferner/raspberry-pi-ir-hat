use crate::hal::gpio::{OutputPin, Pin};

extern crate cortex_m_rt as rt;
extern crate panic_halt;

pub struct IrActivityLedPin {
    pin: Pin,
}

impl IrActivityLedPin {
    pub fn new(pin: Pin) -> IrActivityLedPin {
        return IrActivityLedPin { pin };
    }

    pub fn toggle(&mut self) {
        return self.pin.toggle();
    }
}
