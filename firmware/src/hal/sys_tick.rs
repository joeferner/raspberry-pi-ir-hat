use cortex_m::peripheral::{syst::SystClkSource, SYST};

use super::SYS_CLK;

pub struct SysTick {
    syst: SYST,
}

impl SysTick {
    pub fn new(syst: SYST) -> Self {
        return Self { syst };
    }

    pub fn set_1ms_tick(&mut self) {
        self.syst.set_clock_source(SystClkSource::Core);
        self.syst.set_reload((SYS_CLK / 10) - 1);
        self.syst.clear_current();
        self.syst.enable_counter();
    }

    pub fn get_current(&self) -> u32 {
        return SYST::get_current();
    }
}
