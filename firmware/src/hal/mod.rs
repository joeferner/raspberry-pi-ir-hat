use self::hertz::Hertz;

pub mod baud_rate;
pub mod dma;
pub mod gpio;
pub mod hertz;
pub mod nvic;
pub mod rcc;
pub mod sys_tick;
pub mod syscfg;
pub mod timer;
pub mod usart;

pub const SYS_CLK: Hertz = Hertz::megahertz(16);
