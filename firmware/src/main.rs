#![cfg_attr(not(test), no_main)]
#![cfg_attr(not(test), no_std)]

extern crate cortex_m_rt as rt;

#[cfg(not(test))]
extern crate panic_halt;

use crate::hal::gpio::{gpioa, gpiob, gpioc};
use crate::hal::usart::usart1;
use buffered_io::BufferedIo;
use debug::DebugUsart;
use hal::baud_rate::BaudRate;
use hal::dma::{Dma, DmaMux};
use hal::nvic::NVIC;
use hal::rcc::{ADCClockSource, AHBPrescaler, APB1Prescaler, SysClkSource, USART1ClockSource, RCC};
use hal::sys_tick::SysTick;
use hal::syscfg::SYSCFG;
use hal::timer::{Timer, TimerChannel};
use ir::{IrDecoder, MINIMUM_DEAD_DURATION};
use ir_activity_led_pin::IrActivityLedPin;
use ir_rx::IrRx;
use ir_tx::IrTx;

mod buffered_io;
mod debug;
mod duration;
mod hal;
mod ir;
mod ir_activity_led_pin;
mod ir_rx;
mod ir_tx;

const DEBUG_RX_BUFFER_LEN: usize = 100;

//#[cfg(not(test))]
#[no_mangle]
fn main() -> ! {
    let cortex_m_peripherals = cortex_m::Peripherals::take().unwrap();
    let stm_peripherals = stm32g0::stm32g031::Peripherals::take().unwrap();
    let mut nvic = NVIC::new();
    let mut rcc = RCC::new(stm_peripherals.RCC);
    let mut sys_tick = SysTick::new(cortex_m_peripherals.SYST);
    let mut syscfg = SYSCFG::new(stm_peripherals.SYSCFG);
    rcc.enable_syscfg();
    rcc.enable_power_interface();
    rcc.enable_hsi();
    rcc.enable_lsi();
    sys_tick.set_1ms_tick();
    rcc.set_ahb_prescaler(AHBPrescaler::Div1);
    rcc.set_sys_clk_source(SysClkSource::HSI);
    rcc.set_apb1_prescaler(APB1Prescaler::Div1);
    rcc.set_usart1_clock_source(USART1ClockSource::PCLK);
    rcc.set_adc_clock_source(ADCClockSource::SYSCLK);

    let mut dma = Dma::new(stm_peripherals.DMA, &mut rcc);
    let dma_mux = DmaMux::new(stm_peripherals.DMAMUX).split();

    let ir_rx_timer = Timer::new_timer3(stm_peripherals.TIM3, &mut rcc);
    let ir_rx_timer_channel = TimerChannel::Channel1;

    let ir_tx_carrier_timer = Timer::new_timer17(stm_peripherals.TIM17, &mut rcc);
    let ir_tx_carrier_timer_channel = TimerChannel::Channel1;
    let ir_tx_signal_timer = Timer::new_timer16(stm_peripherals.TIM16, &mut rcc);
    let ir_tx_signal_timer_channel = TimerChannel::Channel1;

    let gpioa = gpioa::new(stm_peripherals.GPIOA, &mut rcc).split();
    let mut ir_activity_led_pin = gpioa.p7;
    ir_activity_led_pin.set_as_output();

    let ir_input_pin = gpioa.p6;

    let gpiob = gpiob::new(stm_peripherals.GPIOB, &mut rcc).split();
    let usart1_tx_pin = gpiob.p6;
    let usart1_rx_pin = gpiob.p7;

    let gpioc = gpioc::new(stm_peripherals.GPIOC, &mut rcc).split();
    let ir_output_pin = gpioc.p14;

    // debug usart
    let mut usart1 = usart1::new(
        stm_peripherals.USART1,
        usart1_tx_pin,
        usart1_rx_pin,
        &mut rcc,
    );
    usart1.set_baud_rate(BaudRate::bps(57_600), &rcc);
    usart1.enable();
    usart1.enable_interrupts(&mut nvic);

    let mut ir_activity_led = IrActivityLedPin::new(ir_activity_led_pin);
    let mut debug = DebugUsart::new(usart1);
    let mut debug_io: BufferedIo<DEBUG_RX_BUFFER_LEN> = BufferedIo::new(&mut debug);

    let mut ir_rx = IrRx::new(
        ir_input_pin,
        ir_rx_timer,
        ir_rx_timer_channel,
        dma_mux.ch5,
        &mut dma,
        &mut rcc,
        &mut nvic,
    );

    let _ir_tx = IrTx::new(
        &mut syscfg,
        &mut rcc,
        ir_output_pin,
        ir_tx_carrier_timer,
        ir_tx_carrier_timer_channel,
        ir_tx_signal_timer,
        ir_tx_signal_timer_channel,
    );

    let mut ir_last_time: Option<u32> = Option::None;
    let mut ir_decoder = IrDecoder::new();

    debug_io.write(b'\n').ok();
    debug_io.write(b'>').ok();
    loop {
        let t = sys_tick.get_current();
        debug_io.tick();
        ir_activity_led.toggle();

        let mut buf = [0u8; 100];
        let s = debug_io.read_line(&mut buf).ok().unwrap();
        if let Option::Some(s) = s {
            debug_io.write_str("in: ").ok();
            debug_io.write_str(s).ok();
            debug_io.write(b'>').ok();
        }

        let ir = ir_rx.read(&dma);
        if let Option::Some(ir) = ir {
            ir_decoder.push(ir);
            ir_last_time = Option::Some(sys_tick.get_current());
        }

        if let Option::Some(last) = ir_last_time {
            if last < t - MINIMUM_DEAD_DURATION.to_milliseconds() {
                if let Option::Some(signal) = ir_decoder.decode() {
                    debug_io.write_str("code: ").ok();
                    debug_io.write_u8(signal.protocol.val()).ok();
                    debug_io.write(b',').ok();
                    debug_io.write_u16(signal.command).ok();
                    debug_io.write(b',').ok();
                    debug_io.write_u16(signal.address).ok();
                    debug_io.write(b'\n').ok();
                }
                ir_decoder.reset();
                ir_last_time = Option::None;
            }
        }
    }
}
