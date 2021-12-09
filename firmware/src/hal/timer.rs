use stm32g0::stm32g031;

use super::rcc::RCC;

pub struct Timer {
    number: u8,
}

impl Timer {
    pub fn new(timer: stm32g031::TIM3, rcc: &mut RCC) -> Self {
        rcc.enable_timer3();
        return Self { number: 3 };
    }

    pub fn get_timer_number(&self) -> u8 {
        return self.number;
    }
}
