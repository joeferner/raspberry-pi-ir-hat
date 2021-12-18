use cortex_m::peripheral::{syst::SystClkSource, SYST};
use cortex_m_rt::exception;

use super::SYS_CLK;

static mut COUNT: u32 = 0;

pub struct SysTick {
    syst: SYST,
}

impl SysTick {
    pub fn new(syst: SYST) -> Self {
        return Self { syst };
    }

    pub fn set_1ms_tick(&mut self) {
        self.syst.set_clock_source(SystClkSource::Core);
        self.syst.set_reload(SYS_CLK.to_hertz() / 1000);
        self.syst.clear_current();
        self.syst.enable_counter();
        self.syst.enable_interrupt();
    }

    pub fn get_current(&self) -> u32 {
        return unsafe { COUNT };
    }
}

#[exception]
fn SysTick() {
    unsafe {
        COUNT += 1;
    }
}
