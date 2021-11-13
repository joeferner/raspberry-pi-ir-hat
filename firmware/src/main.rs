#![no_main]
#![no_std]
extern crate cortex_m;
extern crate cortex_m_rt as runtime;
extern crate stm32g0;

use core::panic::PanicInfo;
use cortex_m::asm;
use debug::debug_init;
use stm32g0::stm32g031;

mod debug;
mod utils;

#[no_mangle]
fn main() -> ! {
    let stm_peripherals = stm32g031::Peripherals::take().unwrap();
    init(&stm_peripherals);
    loop {
        do_loop(&stm_peripherals);
    }
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}

fn init(stm_peripherals: &stm32g031::Peripherals) {
    ir_activity_led_pin_init(&stm_peripherals);
    debug_init(&stm_peripherals);
}

fn do_loop(stm_peripherals: &stm32g031::Peripherals) {
    stm_peripherals
        .GPIOA
        .odr
        .modify(|r, w| w.odr7().bit(r.odr7().bit_is_clear()));

    stm_peripherals.USART1.tdr.write(|w| unsafe { w.bits('a' as u32) });

    for _i in 0..200000 {
        asm::nop()
    }
}

fn ir_activity_led_pin_init(stm_peripherals: &stm32g031::Peripherals) {
    // Enable the clock for GPIOA
    stm_peripherals.RCC.iopenr.write(|w| w.iopaen().bit(true));

    // Configure pin as output
    stm_peripherals.GPIOA.odr.modify(|_, w| w.odr7().set_bit());
}
