pub mod baud_rate;
pub mod dma;
pub mod gpio;
pub mod hertz;
pub mod nvic;
pub mod rcc;
pub mod sys_tick;
pub mod timer;
pub mod usart;

const SYS_CLK: u32 = 16_000_000u32;
