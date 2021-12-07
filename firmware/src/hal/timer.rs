use stm32g0::stm32g031;

pub struct Timer {}

impl Timer {
    pub fn new(timer: stm32g031::TIM3) -> Self {
        return Self {};
    }
}
