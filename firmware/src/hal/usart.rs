use super::{baud_rate::BaudRate, rcc::RCC};
use cortex_m::peripheral::NVIC;
use stm32g0::stm32g031::Interrupt;

pub struct USART {
    register_block: *const stm32g0::stm32g031::usart1::RegisterBlock,
}

pub enum Error {
    WouldBlock,
    Parity,
    Framing,
    Noise,
    Overrun,
}

unsafe impl Send for USART {}

impl USART {
    pub fn set_baud_rate(&mut self, baud_rate: BaudRate, rcc: &RCC) {
        let usart = unsafe { &*self.register_block };
        let clk = rcc.get_usart1_clock_frequency().to_hertz() as u64;
        let bdr = baud_rate.to_bps() as u64;
        let div = clk / bdr / 16;
        usart
            .brr
            .write(|w| unsafe { w.brr_4_15().bits(div as u16) });
    }

    pub fn enable(&mut self) {
        let usart = unsafe { &*self.register_block };
        usart
            .cr1
            .modify(|_, w| w.te().set_bit().re().set_bit());
        usart.cr1.modify(|_, w| w.ue().set_bit());
    }

    pub fn enable_interrupts(&mut self) {
        unsafe {
            if self.register_block == stm32g0::stm32g031::USART1::ptr() {
                NVIC::unmask(Interrupt::USART1);
            } else {
                panic!();
            }
        }
    }

    pub fn listen(&mut self, event: Event) {
        let usart = unsafe { &*self.register_block };
        match event {
            Event::RxFifoNotEmpty => usart.cr1.modify(|_, w| w.rxneie().set_bit()),
            Event::TxFifoNotFull => usart.cr1.modify(|_, w| w.txeie().set_bit()),
        }
    }

    pub fn unlisten(&mut self, event: Event) {
        let usart = unsafe { &*self.register_block };
        match event {
            Event::RxFifoNotEmpty => usart.cr1.modify(|_, w| w.rxneie().clear_bit()),
            Event::TxFifoNotFull => usart.cr1.modify(|_, w| w.txeie().clear_bit()),
        }
    }

    pub fn is_pending(&mut self, event: Event) -> bool {
        let usart = unsafe { &*self.register_block };
        return match event {
            Event::RxFifoNotEmpty => usart.isr.read().rxne().bit_is_set(),
            Event::TxFifoNotFull => usart.isr.read().txe().bit_is_set(),
        };
    }

    pub fn unpend(&mut self, event: Event) {
        let usart = unsafe { &*self.register_block };
        match event {
            Event::RxFifoNotEmpty => panic!(), // should call read to clear this interrupt
            Event::TxFifoNotFull => usart.icr.write(|w| w.txfecf().set_bit()),
        };
    }

    pub fn read(&mut self) -> Result<u8, Error> {
        let usart = unsafe { &*self.register_block };
        let isr = usart.isr.read();
        if isr.pe().bit_is_set() {
            usart.icr.write(|w| w.pecf().set_bit());
            return Result::Err(Error::Parity);
        } else if isr.fe().bit_is_set() {
            usart.icr.write(|w| w.fecf().set_bit());
            return Result::Err(Error::Framing);
        } else if isr.nf().bit_is_set() {
            usart.icr.write(|w| w.ncf().set_bit());
            return Result::Err(Error::Noise);
        } else if isr.ore().bit_is_set() {
            usart.icr.write(|w| w.orecf().set_bit());
            return Result::Err(Error::Overrun);
        } else if isr.rxne().bit_is_set() {
            return Result::Ok(usart.rdr.read().bits() as u8);
        } else {
            return Result::Err(Error::WouldBlock);
        }
    }

    pub fn is_tx_fifo_full(&self) -> bool {
        let usart = unsafe { &*self.register_block };
        return usart.isr.read().txe().bit_is_clear();
    }

    pub fn write(&mut self, b: u8) -> Result<(), Error> {
        if self.is_tx_fifo_full() {
            return Result::Err(Error::WouldBlock);
        }
        let usart = unsafe { &*self.register_block };
        usart.tdr.write(|w| unsafe { w.tdr().bits(b as u16) });
        return Result::Ok(());
    }
}

pub enum Event {
    /// RXFNEIE - RXFIFO in not full, data can be read
    RxFifoNotEmpty,
    /// TXFNFIE - TXFIFO in not full and more data can be written
    TxFifoNotFull,
}

macro_rules! usart {
    ($USARTx:ident, $usartx:ident) => {
        pub mod $usartx {
            use super::USART;
            use crate::hal::gpio::Pin;

            pub fn new(_usart: stm32g0::stm32g031::$USARTx, _tx_pin: Pin, _rx_pin: Pin) -> USART {
                return USART {
                    register_block: stm32g0::stm32g031::$USARTx::ptr(),
                };
            }
        }
    };
}

usart!(USART1, usart1);
