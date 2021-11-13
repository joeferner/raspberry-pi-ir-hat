use crate::utils::GPIO_MODER_AF;
use stm32g0::stm32g031;

pub const USART1_FREQ: u32 = 16_000_000;
pub const USART1_BAUD: u32 = 57_600;
pub const USART1_BRR: u16 = (USART1_FREQ / USART1_BAUD) as u16;

pub fn debug_init(stm_peripherals: &stm32g031::Peripherals) {
    // Enable the clock for USART1
    stm_peripherals
        .RCC
        .apbenr2
        .write(|w| w.usart1en().bit(true));
    stm_peripherals.RCC.iopenr.write(|w| w.iopben().bit(true));

    // Configure PB7 USART1_RX pin as input
    stm_peripherals
        .GPIOB
        .moder
        .modify(|_, w| unsafe { w.moder7().bits(GPIO_MODER_AF) });

    // Configure PB6 USART1_TX pin as output
    stm_peripherals
        .GPIOB
        .moder
        .modify(|_, w| unsafe { w.moder6().bits(GPIO_MODER_AF) });

    // enable IRQ
    unsafe {
        cortex_m::peripheral::NVIC::unmask(stm32g031::Interrupt::USART1);
    }

    // setup USART1
    stm_peripherals
        .USART1
        .brr
        .write(|w| unsafe { w.brr_4_15().bits(USART1_BRR) });

    // enable USART1
    stm_peripherals.USART1.cr3.modify(|_, w| w.iren().set_bit());
}
