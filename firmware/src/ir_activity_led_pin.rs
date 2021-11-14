extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

use hal::gpio::gpioa::PA7;
use hal::gpio::{Analog, Output, PushPull};
use hal::prelude::*;

pub struct IrActivityLedPin {
    pin: PA7<Output<PushPull>>,
}

impl IrActivityLedPin {
    pub fn new(pin: PA7<Analog>) -> IrActivityLedPin {
        let pin = pin.into_push_pull_output();
        return IrActivityLedPin { pin };
    }

    pub fn toggle(&mut self) {
        return self.pin.toggle().unwrap();
    }
}
