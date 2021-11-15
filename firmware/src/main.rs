#![no_main]
#![no_std]

extern crate cortex_m_rt as rt;
extern crate panic_halt;
extern crate stm32g0xx_hal as hal;

use cortex_m::asm;
use debug::DebugUsart;
use hal::prelude::*;
use hal::stm32::{self};
use ir_activity_led_pin::IrActivityLedPin;

mod debug;
mod ir_activity_led_pin;

#[no_mangle]
fn main() -> ! {
    let stm_peripherals = stm32::Peripherals::take().unwrap();
    let mut rcc = stm_peripherals.RCC.constrain();
    let gpioa = stm_peripherals.GPIOA.split(&mut rcc);
    let gpiob = stm_peripherals.GPIOB.split(&mut rcc);

    let mut ir_activity_led = IrActivityLedPin::new(gpioa.pa7);
    let debug = DebugUsart::new(stm_peripherals.USART1, gpiob.pb6, gpiob.pb7, &mut rcc);

    loop {
        ir_activity_led.toggle();
        debug.write("hello world\n").ok();
        for _i in 0..1000000 {
            asm::nop()
        }
    }
}
