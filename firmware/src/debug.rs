extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

use core::str;
use core::str::Bytes;
use core::{cell::RefCell, ops::DerefMut};
use cortex_m::interrupt::Mutex;
use cortex_m::peripheral::NVIC;
use hal::gpio::gpiob::{PB6, PB7};
use hal::gpio::Analog;
use hal::prelude::*;
use hal::rcc::Rcc;
use hal::serial;
use hal::serial::*;
use hal::stm32;
use hal::stm32::interrupt;
use hal::stm32::Interrupt;
use heapless::mpmc::Q32;
use heapless::Deque;

const RX_FIFO_LEN: usize = 128;

static RX_IRQ_FIFO: Q32<u8> = Q32::new();
static TX_IRQ_FIFO: Q32<u8> = Q32::new();

struct Shared {
    serial: Serial<stm32::USART1, FullConfig>,
}

static SHARED: Mutex<RefCell<Option<Shared>>> = Mutex::new(RefCell::new(None));

pub enum DebugError {}

pub enum PeekUntilResult {
    Found(usize),
    EndOfFifo(usize),
    NotFound,
}

pub struct DebugUsart {
    rx_fifo: Deque<u8, RX_FIFO_LEN>,
}

impl DebugUsart {
    pub fn new(
        usart: stm32::USART1,
        tx_pin: PB6<Analog>,
        rx_pin: PB7<Analog>,
        rcc: &mut Rcc,
    ) -> Self {
        let mut serial = usart
            .usart(
                tx_pin,
                rx_pin,
                FullConfig::default().baudrate(57_600.bps()),
                rcc,
            )
            .unwrap();
        serial.listen(serial::Event::Rxne);
        serial.unlisten(serial::Event::Txe);
        cortex_m::interrupt::free(|cs| {
            *SHARED.borrow(cs).borrow_mut() = Some(Shared { serial });
            unsafe {
                NVIC::unmask(Interrupt::USART1);
            }
        });
        return DebugUsart {
            rx_fifo: Deque::new(),
        };
    }

    pub fn write_str(&self, value: &str) -> Result<(), DebugError> {
        return self.write_bytes(&mut value.bytes());
    }

    pub fn write_bytes(&self, value: &mut Bytes) -> Result<(), DebugError> {
        for b in value {
            while let Result::Err(_err) = TX_IRQ_FIFO.enqueue(b) {
                DebugUsart::listen_txe();
            }
        }
        DebugUsart::listen_txe();
        return Result::Ok(());
    }

    fn listen_txe() {
        cortex_m::interrupt::free(|cs| {
            if let Option::Some(shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
                shared.serial.listen(serial::Event::Txe);
            }
        });
    }

    pub fn write(&self, value: u8) -> Result<(), DebugError> {
        while let Result::Err(_err) = TX_IRQ_FIFO.enqueue(value) {
            DebugUsart::listen_txe();
        }
        DebugUsart::listen_txe();
        return Result::Ok(());
    }

    fn fill_rx_fifo(&mut self) {
        while !self.rx_fifo.is_full() {
            match RX_IRQ_FIFO.dequeue() {
                Option::None => {
                    return;
                }
                Option::Some(v) => {
                    self.rx_fifo.push_back(v).unwrap();
                }
            }
        }
    }

    pub fn read(&mut self) -> Option<u8> {
        self.fill_rx_fifo();
        return self.rx_fifo.pop_front();
    }

    pub fn read_bytes(&mut self, len: usize, buffer: &mut [u8]) -> usize {
        for i in 0..len {
            if let Option::Some(v) = self.rx_fifo.pop_front() {
                buffer[i] = v;
            } else {
                return i;
            }
        }
        return len;
    }

    pub fn find(&mut self, stop: u8) -> PeekUntilResult {
        self.fill_rx_fifo();

        let mut read = 0;
        for b in self.rx_fifo.iter() {
            read = read + 1;
            if *b == stop {
                return PeekUntilResult::Found(read);
            }
        }
        if read == self.rx_fifo.len() {
            return PeekUntilResult::EndOfFifo(read);
        } else {
            return PeekUntilResult::NotFound;
        }
    }

    pub fn read_line<'a>(&mut self, buf: &'a mut [u8]) -> Result<Option<&'a str>, DebugError> {
        let len_to_read = match self.find(b'\n') {
            PeekUntilResult::Found(offset) => offset,
            PeekUntilResult::NotFound => {
                return Result::Ok(Option::None);
            }
            PeekUntilResult::EndOfFifo(offset) => offset,
        };
        let l = self.read_bytes(len_to_read, buf);
        unsafe {
            let s = str::from_utf8_unchecked(&buf[0..l]);
            return Result::Ok(Option::Some(s));
        }
    }
}

#[interrupt]
fn USART1() {
    cortex_m::interrupt::free(|cs| {
        if let Some(ref mut shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
            let serial = &mut shared.serial;

            if serial.is_pending(serial::Event::Rxne) {
                RX_IRQ_FIFO.enqueue(serial.read().unwrap()).ok();
                serial.unpend(serial::Event::Rxne);
            }

            if serial.is_pending(serial::Event::Txe) {
                if let Option::Some(v) = TX_IRQ_FIFO.dequeue() {
                    serial.write(v).ok();
                } else {
                    serial.unlisten(serial::Event::Txe);
                }
                serial.unpend(serial::Event::Txe);
            }

            NVIC::unpend(Interrupt::USART1);
        }
    });
}
