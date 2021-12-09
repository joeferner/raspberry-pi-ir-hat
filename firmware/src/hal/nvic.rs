use cortex_m::peripheral;
use stm32g0::stm32g031::Interrupt;

pub struct NVIC {}

impl NVIC {
    pub fn new() -> Self {
        return Self {};
    }

    pub fn enable_interrupt_usart1(&mut self) {
        unsafe {
            peripheral::NVIC::unmask(Interrupt::USART1);
        }
    }

    pub fn enable_interrupt_timer3(&mut self) {
        unsafe {
            peripheral::NVIC::unmask(Interrupt::TIM3);
        }
    }
}
