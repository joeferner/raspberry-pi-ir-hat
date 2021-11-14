#![no_main]
#![no_std]

extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

use cortex_m::asm;
use hal::dma::{Channel, Target};
use hal::gpio::gpioa::PA7;
use hal::gpio::gpiob::{PB6, PB7};
use hal::gpio::{Analog, Output, PushPull};
use hal::rcc::Rcc;
use hal::serial::*;
use hal::stm32::{self, USART1};
use hal::{dma, prelude::*};

#[no_mangle]
fn main() -> ! {
    let stm_peripherals = stm32::Peripherals::take().unwrap();
    let mut rcc = stm_peripherals.RCC.constrain();
    let gpioa = stm_peripherals.GPIOA.split(&mut rcc);
    let gpiob = stm_peripherals.GPIOB.split(&mut rcc);
    let dma = stm_peripherals.DMA.split(&mut rcc, stm_peripherals.DMAMUX);

    let mut ir_activity_led = IrActivityLedPin::new(gpioa.pa7);
    let mut debug = DebugUsart::new(
        stm_peripherals.USART1,
        gpiob.pb6,
        gpiob.pb7,
        dma.ch1,
        &mut rcc,
    );

    loop {
        ir_activity_led.toggle();
        debug.write("hello world\n");
        for _i in 0..1000000 {
            asm::nop()
        }
    }
}

struct DebugUsart {
    usart_tx: Tx<USART1, FullConfig>,
    _usart_rx: Rx<USART1, FullConfig>,
    dma_ch: hal::dma::C1,
    buffer: [u8; 100],
}

impl DebugUsart {
    pub fn new(
        usart: USART1,
        tx_pin: PB6<Analog>,
        rx_pin: PB7<Analog>,
        dma_ch: hal::dma::C1,
        rcc: &mut Rcc,
    ) -> DebugUsart {
        let usart = usart
            .usart(
                tx_pin,
                rx_pin,
                FullConfig::default().baudrate(57_600.bps()),
                rcc,
            )
            .unwrap();
        let (usart_tx, usart_rx) = usart.split();
        return DebugUsart {
            usart_tx,
            _usart_rx: usart_rx,
            dma_ch,
            buffer: [0; 100],
        };
    }

    pub fn write(&mut self, string: &str) {
        let bytes = string.bytes();
        let len = bytes.len();
        for (i, b) in bytes.enumerate() {
            self.buffer[i] = b;
        }

        let usart = unsafe { &(*stm32::USART1::ptr()) };
        let tx_data_register_addr = &usart.tdr as *const _ as u32;
        let tx_dma_buf_addr: u32 = self.buffer.as_ptr() as u32;

        self.dma_ch.disable();
        self.dma_ch.set_direction(dma::Direction::FromMemory);
        self.dma_ch.set_memory_address(tx_dma_buf_addr, true);
        self.dma_ch
            .set_peripheral_address(tx_data_register_addr, false);
        self.dma_ch.set_transfer_length(len as u16);

        self.dma_ch.select_peripheral(self.usart_tx.dmamux());

        self.dma_ch.listen(dma::Event::TransferComplete);

        self.usart_tx.enable_dma();
        self.dma_ch.enable();
    }
}

struct IrActivityLedPin {
    pin: PA7<Output<PushPull>>,
}

impl IrActivityLedPin {
    pub fn new(pin: PA7<Analog>) -> IrActivityLedPin {
        let pin = pin.into_push_pull_output();
        return IrActivityLedPin { pin };
    }

    pub fn toggle(&mut self) {
        return self.pin.toggle().unwrap();
    }
}
