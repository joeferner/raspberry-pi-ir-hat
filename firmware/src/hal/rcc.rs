use core::panic;

use super::hertz::Hertz;

enum ClockSelection {
    PCLK = 0b00,
    SYSCLK = 0b01,
    HSI16 = 0b10,
    LSE = 0b11,
}

impl ClockSelection {
    pub fn from(v: u8) -> ClockSelection {
        return match v {
            0b00 => ClockSelection::PCLK,
            0b01 => ClockSelection::SYSCLK,
            0b10 => ClockSelection::HSI16,
            0b11 => ClockSelection::LSE,
            _ => panic!(),
        };
    }
}

pub struct RCC {
    rcc: stm32g0::stm32g031::RCC,
}

impl RCC {
    pub fn new(rcc: stm32g0::stm32g031::RCC) -> RCC {
        // let (apb_freq, apb_tim_freq, apb_psc_bits) = match rcc_cfg.apb_psc {
        //     Prescaler::Div2 => (sys_freq / 2, sys_freq, 0b100),
        //     Prescaler::Div4 => (sys_freq / 4, sys_freq / 2, 0b101),
        //     Prescaler::Div8 => (sys_freq / 8, sys_freq / 4, 0b110),
        //     Prescaler::Div16 => (sys_freq / 16, sys_freq / 8, 0b111),
        //     _ => (sys_clk.0, sys_clk.0, 0b000),
        // };
        return RCC { rcc };
    }

    pub fn enable_gpioa(&self) {
        self.rcc.iopenr.modify(|_, w| w.iopaen().set_bit());
    }

    pub fn enable_gpiob(&self) {
        self.rcc.iopenr.modify(|_, w| w.iopben().set_bit());
    }

    pub fn enable_usart1(&self) {
        self.rcc.apbenr2.modify(|_, w| w.usart1en().set_bit());
    }

    pub fn get_usart1_clock_frequency(&self) -> Hertz {
        let usart1_clk = ClockSelection::from(self.rcc.ccipr.read().lpuart1sel().bits());
        return match usart1_clk {
            ClockSelection::SYSCLK => self.get_system_clock_frequency(),
            ClockSelection::HSI16 => Hertz::hertz(16_000_000),
            ClockSelection::LSE => Hertz::hertz(32_768),
            ClockSelection::PCLK => self.get_pclk1_clock_frequency(
                self.get_hclk_clock_frequency(self.get_system_clock_frequency()),
            ),
        };
    }

    fn get_system_clock_frequency(&self) -> Hertz {
        // RCC_GetSystemClockFreq
        panic!();
    }

    fn get_hclk_clock_frequency(&self, f: Hertz) -> Hertz {
        panic!();
        // return __LL_RCC_CALC_HCLK_FREQ(f, self.get_ahb_prescaler());
    }

    fn get_pclk1_clock_frequency(&self, f: Hertz) -> Hertz {
        panic!();
        // return __LL_RCC_CALC_PCLK1_FREQ(f, self.get_apb1_prescaler());
    }

    fn get_ahb_prescaler(&self) -> u8 {
        return self.rcc.cfgr.read().hpre().bits();
    }

    fn get_apb1_prescaler(&self) -> u8 {
        return self.rcc.cfgr.read().ppre().bits();
    }
}
