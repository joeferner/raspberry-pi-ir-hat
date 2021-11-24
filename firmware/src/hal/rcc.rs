pub struct RCC {
    rcc: stm32g0::stm32g031::RCC,
}

impl RCC {
    pub fn new(rcc: stm32g0::stm32g031::RCC) -> RCC {
        return RCC { rcc };
    }

    pub fn enable_gpioa(&self) {
        self.rcc.iopenr.modify(|_, w| w.iopaen().set_bit());
    }

    pub fn enable_gpiob(&self) {
        self.rcc.iopenr.modify(|_, w| w.iopben().set_bit());
    }
}
