extern crate cortex_m_rt as rt;

use crate::buffered_io::{BufferedIoError, BufferedIoTarget};
use crate::hal::usart;
use core::str::Bytes;
use core::{cell::RefCell, ops::DerefMut};
use cortex_m::interrupt::Mutex;
use heapless::mpmc::Q32;
use stm32g0::stm32g031::interrupt;

use crate::hal::usart::USART;

static RX_IRQ_FIFO: Q32<u8> = Q32::new();
static TX_IRQ_FIFO: Q32<u8> = Q32::new();

struct Shared {
    usart: USART,
}

static SHARED: Mutex<RefCell<Option<Shared>>> = Mutex::new(RefCell::new(None));

pub struct DebugUsart {}

impl DebugUsart {
    pub fn new(mut usart: USART) -> Self {
        usart.listen(usart::Event::RxFifoNotEmpty);
        usart.unlisten(usart::Event::TxFifoNotFull);
        cortex_m::interrupt::free(|cs| {
            *SHARED.borrow(cs).borrow_mut() = Some(Shared { usart });
        });
        return DebugUsart {};
    }

    fn listen_tx_complete() {
        cortex_m::interrupt::free(|cs| {
            if let Option::Some(shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
                shared.usart.listen(usart::Event::TxFifoNotFull);
            }
        });
    }
}

impl BufferedIoTarget for DebugUsart {
    fn write_bytes(&mut self, value: &mut Bytes) -> Result<(), BufferedIoError> {
        for b in value {
            while let Result::Err(_err) = TX_IRQ_FIFO.enqueue(b) {
                DebugUsart::listen_tx_complete();
            }
        }
        DebugUsart::listen_tx_complete();
        return Result::Ok(());
    }

    fn write(&mut self, value: u8) -> Result<(), BufferedIoError> {
        while let Result::Err(_err) = TX_IRQ_FIFO.enqueue(value) {
            DebugUsart::listen_tx_complete();
        }
        DebugUsart::listen_tx_complete();
        return Result::Ok(());
    }

    fn read(&mut self) -> Option<u8> {
        return RX_IRQ_FIFO.dequeue();
    }
}

#[interrupt]
fn USART1() {
    cortex_m::interrupt::free(|cs| {
        if let Some(ref mut shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
            let serial = &mut shared.usart;

            if serial.is_pending(usart::Event::RxFifoNotEmpty) {
                if let Result::Ok(v) = serial.read() {
                    RX_IRQ_FIFO.enqueue(v).ok();
                }
            }

            if serial.is_pending(usart::Event::TxFifoNotFull) {
                if let Option::Some(v) = TX_IRQ_FIFO.dequeue() {
                    serial.write(v).ok();
                } else {
                    serial.unlisten(usart::Event::TxFifoNotFull);
                }
                serial.unpend(usart::Event::TxFifoNotFull);
            }
        }
    });
}
