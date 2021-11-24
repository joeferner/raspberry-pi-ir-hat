#![no_main]
#![no_std]

extern crate cortex_m_rt as rt;
extern crate panic_halt;

use crate::hal::gpio::{gpioa, gpiob};
use cortex_m::asm;
use hal::rcc::RCC;
use ir_activity_led_pin::IrActivityLedPin;

mod hal;
mod ir_activity_led_pin;

#[no_mangle]
fn main() -> ! {
    let stm_peripherals = stm32g0::stm32g031::Peripherals::take().unwrap();
    let mut rcc = RCC::new(stm_peripherals.RCC);
    let gpioa = gpioa::GPIOA::new(stm_peripherals.GPIOA).split(&mut rcc);
    let _gpiob = gpiob::GPIOB::new(stm_peripherals.GPIOB).split(&mut rcc);
    // let gpioa = stm_peripherals.GPIOA.split(&mut rcc);
    // let gpiob = stm_peripherals.GPIOB.split(&mut rcc);
    // let dma = stm_peripherals.DMA.split(&mut rcc, stm_peripherals.DMAMUX);

    let mut ir_activity_led = IrActivityLedPin::new(gpioa.pa7);
    // let mut debug = DebugUsart::new(stm_peripherals.USART1, gpiob.pb6, gpiob.pb7, &mut rcc);
    // let mut ir_rx = IrRx::new(
    //     stm_peripherals.TIM3,
    //     dma.ch5,
    //     &mut stm_peripherals.DMA,
    //     &mut rcc,
    // );

    loop {
        ir_activity_led.toggle();
        // debug.write_str("hello world!\n").ok();

        // let mut buf = [0u8; 3];
        // let s = debug.read_line(&mut buf).ok().unwrap();
        // if let Option::Some(s) = s {
        //     debug.write(b'>').ok();
        //     debug.write_str(s).ok();
        // }

        for _i in 0..1000000 {
            asm::nop()
        }
    }
}
