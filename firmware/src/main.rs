#![cfg_attr(not(test), no_main)]
#![cfg_attr(not(test), no_std)]

extern crate cortex_m_rt as rt;

#[cfg(not(test))]
extern crate panic_halt;

use crate::hal::gpio::{gpioa, gpiob};
use crate::hal::usart::usart1;
use buffered_io::BufferedIo;
use cortex_m::asm;
use debug::DebugUsart;
use hal::baud_rate::BaudRate;
use hal::gpio::AlternateFunctionMode;
use hal::init_1ms_tick;
use hal::rcc::{ADCClockSource, AHBPrescaler, APB1Prescaler, SysClkSource, USART1ClockSource, RCC};
use ir_activity_led_pin::IrActivityLedPin;

mod buffered_io;
mod debug;
mod hal;
mod ir_activity_led_pin;

const DEBUG_RX_BUFFER_LEN: usize = 100;

#[cfg(not(test))]
#[no_mangle]
fn main() -> ! {
    let mut cortex_m_peripherals = cortex_m::Peripherals::take().unwrap();
    let stm_peripherals = stm32g0::stm32g031::Peripherals::take().unwrap();
    let mut rcc = RCC::new(stm_peripherals.RCC);
    rcc.enable_syscfg();
    rcc.enable_pwd();
    rcc.enable_hsi();
    rcc.enable_lsi();
    rcc.set_ahb_prescaler(AHBPrescaler::Div1);
    rcc.set_sys_clk_source(SysClkSource::HSI);
    rcc.set_apb1_prescaler(APB1Prescaler::Div1);
    init_1ms_tick(&mut cortex_m_peripherals.SYST);
    rcc.set_usart1_clock_source(USART1ClockSource::PCLK);
    rcc.set_adc_clock_source(ADCClockSource::SYSCLK);
    rcc.enable_gpioa();
    rcc.enable_gpiob();
    rcc.enable_usart1();

    let gpioa = gpioa::new(stm_peripherals.GPIOA).split();
    let mut ir_activity_led_pin = gpioa.p7;
    ir_activity_led_pin.set_as_output();

    let gpiob = gpiob::new(stm_peripherals.GPIOB).split();
    let mut usart1_tx_pin = gpiob.p6;
    let mut usart1_rx_pin = gpiob.p7;
    usart1_tx_pin.set_as_alternate_function(AlternateFunctionMode::AF0);
    usart1_tx_pin.set_output_type_push_pull();
    usart1_tx_pin.set_speed_low();
    usart1_tx_pin.set_pull_none();

    usart1_rx_pin.set_as_alternate_function(AlternateFunctionMode::AF0);
    usart1_rx_pin.set_output_type_push_pull();
    usart1_rx_pin.set_speed_low();
    usart1_rx_pin.set_pull_none();

    // debug usart
    let mut usart1 = usart1::new(stm_peripherals.USART1, usart1_tx_pin, usart1_rx_pin);
    usart1.set_baud_rate(BaudRate::bps(57_600), &rcc);
    usart1.enable();
    usart1.enable_interrupts();

    // let dma = stm_peripherals.DMA.split(&mut rcc, stm_peripherals.DMAMUX);

    let mut ir_activity_led = IrActivityLedPin::new(ir_activity_led_pin);
    let mut debug = DebugUsart::new(usart1);
    let mut debugIo: BufferedIo<DEBUG_RX_BUFFER_LEN> = BufferedIo::new(&mut debug);
    // let mut ir_rx = IrRx::new(
    //     stm_peripherals.TIM3,
    //     dma.ch5,
    //     &mut stm_peripherals.DMA,
    //     &mut rcc,
    // );

    debugIo.write(b'\n').ok();
    debugIo.write(b'>').ok();
    loop {
        debugIo.tick();
        ir_activity_led.toggle();

        let mut buf = [0u8; 100];
        let s = debugIo.read_line(&mut buf).ok().unwrap();
        if let Option::Some(s) = s {
            debugIo.write_str("in: ").ok();
            debugIo.write_str(s).ok();
            debugIo.write(b'>').ok();
        }
    }
}
