extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

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
use heapless::Deque;

const RX_FIFO_LEN: usize = 100;
const TX_FIFO_LEN: usize = 100;

struct Shared {
    serial: Serial<stm32::USART1, FullConfig>,
    rx_fifo: Deque<u8, RX_FIFO_LEN>,
    tx_fifo: Deque<u8, TX_FIFO_LEN>,
}

static SHARED: Mutex<RefCell<Option<Shared>>> = Mutex::new(RefCell::new(None));

pub enum DebugError {
    BufferOverflow,
    LockFail,
}

pub struct DebugUsart {}

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
            *SHARED.borrow(cs).borrow_mut() = Some(Shared {
                serial,
                rx_fifo: Deque::<_, RX_FIFO_LEN>::new(),
                tx_fifo: Deque::<_, TX_FIFO_LEN>::new(),
            });
            unsafe {
                NVIC::unmask(Interrupt::USART1);
            }
        });
        return DebugUsart {};
    }

    pub fn write_str(&self, value: &str) -> Result<(), DebugError> {
        return self.write_bytes(&mut value.bytes());
    }

    pub fn write_bytes(&self, value: &mut Bytes) -> Result<(), DebugError> {
        return cortex_m::interrupt::free(|cs| match SHARED.borrow(cs).borrow_mut().deref_mut() {
            Option::Some(ref mut shared) => {
                for b in value {
                    shared
                        .tx_fifo
                        .push_back(b)
                        .map_err(|_err| DebugError::BufferOverflow)?;
                }
                shared.serial.listen(serial::Event::Txe);
                Result::Ok(())
            }
            Option::None => Result::Err(DebugError::LockFail),
        });
    }

    pub fn read(&self) -> Result<Option<u8>, DebugError> {
        return cortex_m::interrupt::free(|cs| match SHARED.borrow(cs).borrow_mut().deref_mut() {
            Option::Some(ref mut shared) => {
                return Result::Ok(shared.rx_fifo.pop_front());
            }
            Option::None => Result::Err(DebugError::LockFail),
        });
    }

    pub fn read_line(&self, buffer: &mut [u8]) -> Result<Option<u8>, DebugError> {
        return cortex_m::interrupt::free(|cs| match SHARED.borrow(cs).borrow_mut().deref_mut() {
            Option::Some(ref mut shared) => {
                return Result::Ok(shared.rx_fifo.pop_front());
            }
            Option::None => Result::Err(DebugError::LockFail),
        });
    }
}

#[interrupt]
fn USART1() {
    cortex_m::interrupt::free(|cs| {
        if let Some(ref mut shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
            let serial = &mut shared.serial;

            if serial.is_pending(serial::Event::Rxne) {
                shared.rx_fifo.push_back(serial.read().unwrap()).ok();
                serial.unpend(serial::Event::Rxne);
            }

            if serial.is_pending(serial::Event::Txe) {
                if let Option::Some(v) = shared.tx_fifo.pop_front() {
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
