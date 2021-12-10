use core::panic;

use super::hertz::Hertz;

const AHB_PRESCALER_TABLE: [u32; 16] = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9];
const APB_PRESCALER_TABLE: [u32; 8] = [0, 0, 0, 0, 1, 2, 3, 4];

pub enum AHBPrescaler {
    Div1,
}

pub enum APB1Prescaler {
    Div1,
}

pub enum USART1ClockSource {
    PCLK = 0b00,
    SYSCLK = 0b01,
    HSI16 = 0b10,
    LSE = 0b11,
}

impl USART1ClockSource {
    pub fn val(self) -> u8 {
        return self as u8;
    }

    pub fn from(v: u8) -> USART1ClockSource {
        return match v {
            0b00 => USART1ClockSource::PCLK,
            0b01 => USART1ClockSource::SYSCLK,
            0b10 => USART1ClockSource::HSI16,
            0b11 => USART1ClockSource::LSE,
            _ => panic!(),
        };
    }
}

pub enum ADCClockSource {
    SYSCLK = 0b00,
}

impl ADCClockSource {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum SysClkSource {
    HSI = 0b000,
}

impl SysClkSource {
    pub fn val(self) -> u8 {
        return self as u8;
    }

    pub fn from(v: u8) -> SysClkSource {
        return match v {
            0b000 => SysClkSource::HSI,
            _ => panic!(),
        };
    }
}

pub struct RCC {
    rcc: stm32g0::stm32g031::RCC,
}

impl RCC {
    pub fn new(rcc: stm32g0::stm32g031::RCC) -> RCC {
        return RCC { rcc };
    }

    pub fn enable_gpioa(&mut self) {
        self.rcc.iopenr.modify(|_, w| w.iopaen().set_bit());
    }

    pub fn enable_gpiob(&mut self) {
        self.rcc.iopenr.modify(|_, w| w.iopben().set_bit());
    }

    pub fn enable_usart1(&mut self) {
        self.rcc.apbenr2.modify(|_, w| w.usart1en().set_bit());
    }

    pub fn enable_dma1(&mut self) {
        self.rcc.ahbenr.modify(|_, w| w.dmaen().set_bit());
    }

    pub fn enable_timer3(&mut self) {
        self.rcc.apbenr1.modify(|_, w| w.tim3en().set_bit());
    }

    pub fn enable_syscfg(&mut self) {
        self.rcc.apbenr2.modify(|_, w| w.syscfgen().set_bit());
    }

    pub fn enable_pwd(&mut self) {
        self.rcc.apbenr1.modify(|_, w| w.pwren().set_bit());
    }

    pub fn enable_hsi(&mut self) {
        self.rcc.cr.modify(|_, w| w.hsion().set_bit());
        while self.rcc.cr.read().hsirdy().bit_is_clear() {}
    }

    pub fn enable_lsi(&mut self) {
        self.rcc.csr.modify(|_, w| w.lsion().set_bit());
        while self.rcc.csr.read().lsirdy().bit_is_clear() {}
    }

    pub fn set_ahb_prescaler(&mut self, v: AHBPrescaler) {
        match v {
            AHBPrescaler::Div1 => self
                .rcc
                .cfgr
                .modify(|_, w| unsafe { w.hpre().bits(0b0000) }),
        }
    }

    pub fn set_apb1_prescaler(&mut self, v: APB1Prescaler) {
        match v {
            APB1Prescaler::Div1 => self.rcc.cfgr.modify(|_, w| unsafe { w.ppre().bits(0b000) }),
        }
    }

    pub fn set_sys_clk_source(&mut self, v: SysClkSource) {
        let v = v.val();
        self.rcc.cfgr.modify(|_, w| unsafe { w.sw().bits(v) });
        while self.rcc.cfgr.read().sws().bits() != v {}
    }

    pub fn set_usart1_clock_source(&mut self, v: USART1ClockSource) {
        self.rcc
            .ccipr
            .modify(|_, w| unsafe { w.usart1sel().bits(v.val()) });
    }

    pub fn set_adc_clock_source(&mut self, v: ADCClockSource) {
        self.rcc
            .ccipr
            .modify(|_, w| unsafe { w.adcsel().bits(v.val()) });
    }

    pub fn get_usart1_clock_frequency(&self) -> Hertz {
        // LL_RCC_GetUSARTClockFreq
        let usart1_clk = USART1ClockSource::from(self.rcc.ccipr.read().usart1sel().bits());
        return match usart1_clk {
            USART1ClockSource::SYSCLK => self.get_system_clock_frequency(),
            USART1ClockSource::HSI16 => Hertz::megahertz(16),
            USART1ClockSource::LSE => Hertz::hertz(32_768),
            USART1ClockSource::PCLK => self.get_pclk1_clock_frequency(
                self.get_hclk_clock_frequency(self.get_system_clock_frequency()),
            ),
        };
    }

    fn get_system_clock_frequency(&self) -> Hertz {
        // RCC_GetSystemClockFreq
        match self.get_sysclk_source() {
            SysClkSource::HSI => {
                let hsidiv: u32 = (self.rcc.cr.read().hsidiv().bits() + 1) as u32;
                return Hertz::hertz(16_000_000u32 / hsidiv);
            }
        }
    }

    fn get_sysclk_source(&self) -> SysClkSource {
        // LL_RCC_GetSysClkSource
        return SysClkSource::from(self.rcc.cfgr.read().sws().bits());
    }

    fn get_hclk_clock_frequency(&self, f: Hertz) -> Hertz {
        // RCC_GetHCLKClockFreq
        return Hertz::hertz(
            f.to_hertz() >> (AHB_PRESCALER_TABLE[self.get_ahb_prescaler() as usize] & 0x1f),
        );
    }

    fn get_pclk1_clock_frequency(&self, f: Hertz) -> Hertz {
        // RCC_GetPCLK1ClockFreq
        return Hertz::hertz(
            f.to_hertz() >> (APB_PRESCALER_TABLE[self.get_apb1_prescaler() as usize] & 0x1f),
        );
    }

    fn get_ahb_prescaler(&self) -> u8 {
        return self.rcc.cfgr.read().hpre().bits();
    }

    fn get_apb1_prescaler(&self) -> u8 {
        return self.rcc.cfgr.read().ppre().bits();
    }

    pub fn get_system_core_clock(&self) -> Hertz {
        // SystemCoreClock
        return Hertz::megahertz(16); // TODO where is this from 
    }
}
