use core::{cell::RefCell, ops::DerefMut};

use crate::{
    duration::Duration,
    hal::{
        gpio::{Mode, OutputPin, Pin, PortName},
        hertz::Hertz,
        rcc::RCC,
        syscfg::{IrModulationEnvelopeSignal, IrPolarity, SYSCFG},
        timer::{
            Timer, TimerChannel, TimerClockDivision, TimerEvent, TimerNumber, TimerOcIdleState,
            TimerOcMode, TimerOcPolarity,
        },
        SYS_CLK,
    },
};
use cortex_m::interrupt::Mutex;
use heapless::mpmc::MpMcQueue;
use stm32g0::stm32g031::interrupt;

const SIGNAL_TIMER_PRESCALER: u32 = 10;
const CARRIER_TIMER_PRESCALER: u32 = 0;

pub struct IrTx {
    output_pin: Pin,
    carrier_timer: Timer,
    carrier_timer_channel: TimerChannel,
    signal_timer: Timer,
    signal_timer_channel: TimerChannel,
    sending: bool,
    buffer: MpMcQueue<u16, 128>,
}

static SHARED: Mutex<RefCell<Option<IrTx>>> = Mutex::new(RefCell::new(None));

impl IrTx {
    pub fn new(
        syscfg: &mut SYSCFG,
        rcc: &mut RCC,
        output_pin: Pin,
        mut carrier_timer: Timer,
        carrier_timer_channel: TimerChannel,
        mut signal_timer: Timer,
        signal_timer_channel: TimerChannel,
    ) -> Self {
        debug_assert_eq!(14, output_pin.get_pin());
        debug_assert_eq!(PortName::C, output_pin.get_port_name());
        debug_assert_eq!(TimerNumber::Timer17, carrier_timer.get_timer_number());
        debug_assert_eq!(TimerChannel::Channel1, carrier_timer_channel);
        debug_assert_eq!(TimerNumber::Timer16, signal_timer.get_timer_number());
        debug_assert_eq!(TimerChannel::Channel1, signal_timer_channel);

        syscfg.set_ir_modulation_envelope_signal(IrModulationEnvelopeSignal::TIM16);
        syscfg.set_ir_polarity(IrPolarity::Inverted);

        signal_timer.set_prescaler(SIGNAL_TIMER_PRESCALER);
        signal_timer.set_auto_reload(65535);
        signal_timer.set_clock_division(TimerClockDivision::Div1);
        signal_timer.set_repetition_counter(0);
        signal_timer.disable_auto_reload_preload();
        signal_timer.oc_disable_preload(signal_timer_channel);
        signal_timer.oc_set_mode(signal_timer_channel, TimerOcMode::PWM1);
        signal_timer.disable_capture_compare_channel(signal_timer_channel);
        signal_timer.oc_set_compare(signal_timer_channel, 0);
        signal_timer.oc_set_polarity(signal_timer_channel, TimerOcPolarity::High);
        signal_timer.oc_set_npolarity(signal_timer_channel, TimerOcPolarity::High);
        signal_timer.oc_set_idle_state(signal_timer_channel, TimerOcIdleState::Low);
        signal_timer.oc_set_nidle_state(signal_timer_channel, TimerOcIdleState::Low);
        signal_timer.oc_disable_fast_mode(signal_timer_channel);

        // TODO do we need this
        // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
        // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
        // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
        // TIM_BDTRInitStruct.DeadTime = 0;
        // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
        // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
        // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
        // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
        // LL_TIM_BDTR_Init(TIM16, &TIM_BDTRInitStruct);

        carrier_timer.set_prescaler(CARRIER_TIMER_PRESCALER);
        carrier_timer.set_auto_reload(65535);
        carrier_timer.set_clock_division(TimerClockDivision::Div1);
        carrier_timer.set_repetition_counter(0);
        carrier_timer.disable_auto_reload_preload();
        carrier_timer.oc_enable_preload(carrier_timer_channel);
        carrier_timer.oc_set_mode(carrier_timer_channel, TimerOcMode::PWM1);
        carrier_timer.disable_capture_compare_channel(carrier_timer_channel);
        carrier_timer.oc_set_compare(carrier_timer_channel, 0);
        carrier_timer.oc_set_polarity(carrier_timer_channel, TimerOcPolarity::High);
        carrier_timer.oc_set_npolarity(carrier_timer_channel, TimerOcPolarity::High);
        carrier_timer.oc_set_idle_state(carrier_timer_channel, TimerOcIdleState::Low);
        carrier_timer.oc_set_nidle_state(carrier_timer_channel, TimerOcIdleState::Low);
        carrier_timer.oc_disable_fast_mode(carrier_timer_channel);

        // TODO do we need this
        // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
        // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
        // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
        // TIM_BDTRInitStruct.DeadTime = 0;
        // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
        // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
        // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
        // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
        // LL_TIM_BDTR_Init(TIM17, &TIM_BDTRInitStruct);

        let mut result = Self {
            output_pin,
            carrier_timer,
            carrier_timer_channel,
            signal_timer,
            signal_timer_channel,
            sending: false,
            buffer: MpMcQueue::new(),
        };
        result.enable_gpio(false);

        // Not sure why we need this but the first transmit is always wrong
        result.reset(Hertz::kilohertz(38));
        result
            .write(Duration::microseconds(1000), Duration::microseconds(1000))
            .ok();
        result.send();

        return result;
    }

    fn enable_gpio(&mut self, enable: bool) {
        // TODO see if we can adjust idle value of timer output channel to be high
        if enable {
            self.output_pin.set_mode(Mode::AlternateFunction);
            self.output_pin.set_value(true);
        } else {
            self.output_pin.set_as_output();
            self.output_pin.set_value(false);
        }
    }

    fn reset(&mut self, carrier_frequency: Hertz) {
        self.stop();

        // init carrier timer
        let auto_reload =
            Timer::calculate_auto_reload(SYS_CLK, CARRIER_TIMER_PRESCALER, carrier_frequency);
        self.carrier_timer.set_prescaler(CARRIER_TIMER_PRESCALER);
        self.carrier_timer.set_auto_reload(auto_reload);
        self.carrier_timer
            .enable_capture_compare_channel(self.carrier_timer_channel);
        // 25% duty cycle
        self.carrier_timer
            .oc_set_compare(self.carrier_timer_channel, auto_reload / 4);
        self.carrier_timer.enable_all_outputs();

        // init signal timer
        self.signal_timer.set_prescaler(SIGNAL_TIMER_PRESCALER);
        self.signal_timer.set_auto_reload(65000);
        self.signal_timer
            .oc_disable_preload(self.signal_timer_channel);
        self.signal_timer.listen(TimerEvent::Active);
        self.signal_timer
            .enable_capture_compare_channel(self.signal_timer_channel);
        self.signal_timer
            .oc_set_compare(self.signal_timer_channel, 30000);
        self.signal_timer.enable_all_outputs();
    }

    fn write(&mut self, t_on: Duration, t_off: Duration) -> Result<(), u16> {
        self.buffer.enqueue(t_on.to_microseconds() as u16)?;
        self.buffer.enqueue(t_off.to_microseconds() as u16)?;
        return Result::Ok(());
    }

    fn send(&mut self) {
        if !self.sending {
            self.sending = true;
            self.next_signal();
            self.enable_gpio(true);
            self.carrier_timer.enable_counter();
            self.signal_timer.enable_counter();
        }
    }

    fn stop(&mut self) {
        self.sending = false;
        self.signal_timer.disable_counter();
        self.carrier_timer.disable_counter();
        self.enable_gpio(false);
        while let Option::Some(_) = self.buffer.dequeue() {}
    }

    fn next_signal(&mut self) {
        let t_on = self.buffer.dequeue();
        let t_off = self.buffer.dequeue();

        if t_on.is_none() || t_off.is_none() {
            self.stop();
            return;
        }
        let t_on = Timer::calculate_delay(SYS_CLK, SIGNAL_TIMER_PRESCALER, t_on.unwrap());
        let t_off = Timer::calculate_delay(SYS_CLK, SIGNAL_TIMER_PRESCALER, t_off.unwrap());
        let t_total = t_on + t_off;
        self.signal_timer.set_auto_reload(t_total);
        self.signal_timer
            .oc_set_compare(self.signal_timer_channel, t_on);
    }

    fn handle_interrupt(&mut self) {
        if self.signal_timer.is_pending(TimerEvent::Active) {
            self.signal_timer.unpend(TimerEvent::Active);
            self.next_signal();
        }
    }
}

#[interrupt]
fn TIM16() {
    cortex_m::interrupt::free(|cs| {
        if let Option::Some(shared) = SHARED.borrow(cs).borrow_mut().deref_mut() {
            shared.handle_interrupt();
        }
    });
}
