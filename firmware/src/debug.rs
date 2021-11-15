extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

use hal::dma::{Channel, Target};
use hal::gpio::gpiob::{PB6, PB7};
use hal::gpio::Analog;
use hal::rcc::Rcc;
use hal::serial::*;
use hal::stm32::interrupt;
use hal::stm32::{self, USART1};
use hal::{dma, prelude::*};
use heapless::Deque;

const RX_FIFO_LEN: usize = 100;
static RX_FIFO: Deque<u8, RX_FIFO_LEN> = Deque::<_, RX_FIFO_LEN>::new();

struct Shared {
    serial: Serial<stm32::USART1, gpioa::PA9<Alternate<AF1>>, gpioa::PA10<Alternate<AF1>>>,
}

static SHARED: Mutex<RefCell<Option<Shared>>> = Mutex::new(RefCell::new(None));

pub enum DebugError {
    BufferOverflow,
}

pub struct DebugUsart {
}

impl DebugUsart {
    pub fn new(
        usart: USART1,
        tx_pin: PB6<Analog>,
        rx_pin: PB7<Analog>,
        dma_ch: hal::dma::C1,
        rcc: &mut Rcc,
    ) -> Self {
        let usart = usart
            .usart(
                tx_pin,
                rx_pin,
                FullConfig::default().baudrate(57_600.bps()),
                rcc,
            )
            .unwrap();
        cortex_m::interrupt::free(|cs| {
            *SHARED.borrow(cs).borrow_mut() = Some(Shared { usart });
        });
        return DebugUsart {};
    }

    pub fn write(&mut self, string: &str) -> Result<(), DebugError> {
        if self.waiting_for_tx_transfer {
            loop {
                if self.dma_ch.event_occurred(dma::Event::TransferComplete)
                    || self.dma_ch.event_occurred(dma::Event::TransferError)
                {
                    break;
                }
            }
        }

        let bytes = string.bytes();
        let len = bytes.len();
        if len > self.tx_buffer.len() {
            return Result::Err(DebugError::BufferOverflow);
        }
        for (i, b) in bytes.enumerate() {
            self.tx_buffer[i] = b;
        }

        let usart = unsafe { &(*stm32::USART1::ptr()) };
        let tx_data_register_addr = &usart.tdr as *const _ as u32;
        let tx_dma_buf_addr: u32 = self.tx_buffer.as_ptr() as u32;

        self.dma_ch.disable();
        self.dma_ch.set_direction(dma::Direction::FromMemory);
        self.dma_ch.set_memory_address(tx_dma_buf_addr, true);
        self.dma_ch
            .set_peripheral_address(tx_data_register_addr, false);
        self.dma_ch.set_transfer_length(len as u16);

        self.dma_ch.select_peripheral(self.usart_tx.dmamux());

        self.waiting_for_tx_transfer = true;
        self.dma_ch.listen(dma::Event::TransferComplete);
        self.dma_ch.listen(dma::Event::TransferError);

        self.usart_tx.enable_dma();
        self.dma_ch.enable();

        return Result::Ok(());
    }
}

#[interrupt]
fn USART1() {
    cortex_m::interrupt::free(|cs| {
        // Obtain all Mutex protected resources
        if let Some(ref mut shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
            let serial = &mut shared.serial;

            // Read received character
            let received = serial.read().unwrap();

            // Write character back
            serial.write(received).ok();

            // Clear interrupt
            cortex_m::peripheral::NVIC::unpend(USART1);
        }
    });
}
