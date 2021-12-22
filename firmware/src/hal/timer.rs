use stm32g0::stm32g031::{self};

use super::{hertz::Hertz, rcc::RCC, SYS_CLK};

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum TimerCounterDirection {
    Up,
    Down,
}

pub enum TimerCenterAlignMode {
    Edge = 0b00,
}

impl TimerCenterAlignMode {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerClockDivision {
    Div1 = 0b00,
}

impl TimerClockDivision {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerTriggerOutput {
    Reset = 0b0000,
}

impl TimerTriggerOutput {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum TimerChannel {
    Channel1,
}

pub enum TimerOcMode {
    PWM1 = 0b0110,
}

impl TimerOcMode {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerOcPolarity {
    High,
}

pub enum TimerOcIdleState {
    Low,
}

pub enum TimerActiveInput {
    DirectTI = 0b01,
}

impl TimerActiveInput {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerPrescaler {
    Div1 = 0b00,
}

impl TimerPrescaler {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerFilter {
    Div1 = 0b0000,
}

impl TimerFilter {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum TimerPolarity {
    BothEdges,
}

pub enum TimerEvent {
    Active,
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum TimerNumber {
    Timer3,
    Timer16,
    Timer17,
}

pub struct Timer {
    number: TimerNumber,
}

macro_rules! do_with_timer {
    ($self:ident, $timer:ident, $action:expr) => {
        match $self.number {
            TimerNumber::Timer3 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
                $action;
            }
            TimerNumber::Timer16 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM16::ptr() };
                $action;
            }
            TimerNumber::Timer17 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM17::ptr() };
                $action;
            }
        }
    };
}

macro_rules! do_with_timer_3 {
    ($self:ident, $timer:ident, $action:expr) => {
        match $self.number {
            TimerNumber::Timer3 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
                $action;
            }
            TimerNumber::Timer16 => {
                panic!();
            }
            TimerNumber::Timer17 => {
                panic!();
            }
        }
    };
}

macro_rules! do_with_timer_16_17 {
    ($self:ident, $timer:ident, $action:expr) => {
        match $self.number {
            TimerNumber::Timer3 => {
                panic!();
            }
            TimerNumber::Timer16 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM16::ptr() };
                $action;
            }
            TimerNumber::Timer17 => {
                let $timer = unsafe { &*stm32g0::stm32g031::TIM17::ptr() };
                $action;
            }
        }
    };
}

impl Timer {
    pub fn new_timer3(_timer: stm32g031::TIM3, rcc: &mut RCC) -> Self {
        rcc.enable_timer3();
        return Self {
            number: TimerNumber::Timer3,
        };
    }

    pub fn new_timer16(_timer: stm32g031::TIM16, rcc: &mut RCC) -> Self {
        rcc.enable_timer16();
        return Self {
            number: TimerNumber::Timer16,
        };
    }

    pub fn new_timer17(_timer: stm32g031::TIM17, rcc: &mut RCC) -> Self {
        rcc.enable_timer17();
        return Self {
            number: TimerNumber::Timer17,
        };
    }

    pub fn get_timer_number(&self) -> TimerNumber {
        return self.number;
    }

    pub fn disable_auto_reload_preload(&mut self) {
        // LL_TIM_DisableARRPreload
        do_with_timer!(self, timer, timer.cr1.modify(|_, w| w.arpe().clear_bit()));
    }

    pub fn set_trigger_output(&mut self, o: TimerTriggerOutput) {
        // LL_TIM_SetTriggerOutput
        do_with_timer_3!(
            self,
            timer,
            timer.cr2.modify(|_, w| unsafe { w.mms().bits(o.val()) })
        );
    }

    pub fn disable_master_slave_mode(&mut self) {
        // LL_TIM_DisableMasterSlaveMode
        do_with_timer_3!(self, timer, timer.smcr.modify(|_, w| w.msm().clear_bit()));
    }

    pub fn ic_set_active_input(&mut self, channel: TimerChannel, input: TimerActiveInput) {
        // LL_TIM_IC_SetActiveInput
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.cc1s().bits(input.val()) }),
            }
        );
    }

    pub fn ic_set_prescaler(&mut self, channel: TimerChannel, prescaler: TimerPrescaler) {
        // LL_TIM_IC_SetPrescaler
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.ic1psc().bits(prescaler.val()) }),
            }
        );
    }

    pub fn ic_set_filter(&mut self, channel: TimerChannel, filter: TimerFilter) {
        // LL_TIM_IC_SetFilter
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.ic1f().bits(filter.val()) }),
            }
        );
    }

    pub fn ic_set_polarity(&mut self, channel: TimerChannel, polarity: TimerPolarity) {
        // LL_TIM_IC_SetPolarity
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerPolarity::BothEdges => w.cc1np().set_bit().cc1p().set_bit(),
                }),
            }
        );
    }

    pub fn oc_set_compare(&mut self, channel: TimerChannel, value: u32) {
        // LL_TIM_OC_SetCompareCH1
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccr1.modify(|_, w| unsafe { w.bits(value) }),
            }
        );
    }

    pub fn oc_disable_preload(&mut self, channel: TimerChannel) {
        // LL_TIM_OC_DisablePreload
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1pe().clear_bit()),
            }
        );
    }

    pub fn oc_set_mode(&mut self, channel: TimerChannel, mode: TimerOcMode) {
        // LL_TIM_OC_SetMode
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_output()
                    .modify(|_, w| w.oc1m().bits(mode.val())),
            }
        );
    }

    pub fn oc_set_polarity(&mut self, channel: TimerChannel, polarity: TimerOcPolarity) {
        // LL_TIM_OC_SetPolarity
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerOcPolarity::High => {
                        w.cc1p().clear_bit()
                    }
                }),
            }
        );
    }

    pub fn oc_set_npolarity(&mut self, channel: TimerChannel, polarity: TimerOcPolarity) {
        // LL_TIM_OC_SetPolarity
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerOcPolarity::High => {
                        w.cc1np().clear_bit()
                    }
                }),
            }
        );
    }

    pub fn oc_set_idle_state(&mut self, channel: TimerChannel, idle_state: TimerOcIdleState) {
        // LL_TIM_OC_SetIdleState
        do_with_timer_16_17!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.cr2.modify(|_, w| match idle_state {
                    TimerOcIdleState::Low => {
                        w.ois1().clear_bit()
                    }
                }),
            }
        );
    }

    pub fn oc_set_nidle_state(&mut self, channel: TimerChannel, idle_state: TimerOcIdleState) {
        // LL_TIM_OC_SetIdleState
        do_with_timer_16_17!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.cr2.modify(|_, w| match idle_state {
                    TimerOcIdleState::Low => {
                        w.ois1n().clear_bit()
                    }
                }),
            }
        );
    }

    pub fn oc_disable_fast_mode(&mut self, channel: TimerChannel) {
        // LL_TIM_OC_DisableFast
        do_with_timer_16_17!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1fe().clear_bit()),
            }
        );
    }

    pub fn oc_enable_preload(&mut self, channel: TimerChannel) {
        // LL_TIM_OC_EnablePreload
        do_with_timer_16_17!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1pe().set_bit()),
            }
        );
    }

    pub fn set_counter_direction(&mut self, dir: TimerCounterDirection) {
        // LL_TIM_SetCounterMode
        do_with_timer_3!(
            self,
            timer,
            timer
                .cr1
                .modify(|_, w| w.dir().bit(dir == TimerCounterDirection::Down))
        );
    }

    pub fn set_center_align_mode(&mut self, mode: TimerCenterAlignMode) {
        do_with_timer_3!(
            self,
            timer,
            timer.cr1.modify(|_, w| unsafe { w.cms().bits(mode.val()) })
        );
    }

    pub fn set_clock_division(&mut self, div: TimerClockDivision) {
        do_with_timer!(
            self,
            timer,
            timer.cr1.modify(|_, w| unsafe { w.ckd().bits(div.val()) })
        );
    }

    pub fn set_repetition_counter(&mut self, count: u32) {
        // LL_TIM_SetRepetitionCounter
        do_with_timer_16_17!(
            self,
            timer,
            timer.rcr.modify(|_, w| unsafe { w.bits(count) })
        );
    }

    pub fn set_auto_reload(&mut self, reload: u32) {
        // LL_TIM_SetAutoReload
        do_with_timer!(
            self,
            timer,
            timer.arr.modify(|_, w| unsafe { w.bits(reload) })
        );
    }

    pub fn set_prescaler(&mut self, prescaler: u32) {
        // LL_TIM_SetPrescaler
        do_with_timer!(
            self,
            timer,
            timer.psc.modify(|_, w| unsafe { w.bits(prescaler) })
        );
    }

    pub fn generate_event_update(&mut self) {
        // LL_TIM_GenerateEvent_UPDATE
        do_with_timer!(self, timer, timer.egr.write(|w| w.ug().set_bit()));
    }

    pub fn set_prescaler_hertz(&mut self, hertz: Hertz, _rcc: &RCC) {
        // __LL_TIM_CALC_PSC
        let sys_clk = SYS_CLK.to_hertz();
        let clk = hertz.to_hertz();
        let prescaler = if sys_clk >= clk {
            (sys_clk / clk) - 1
        } else {
            0
        };
        self.set_prescaler(prescaler);
    }

    pub fn get_capture_compare_register_address(&self) -> u32 {
        do_with_timer!(self, timer, return &timer.ccr1 as *const _ as u32);
    }

    pub fn enable_capture_compare_interrupt(&mut self, channel: TimerChannel) {
        // LL_TIM_EnableIT_CC1
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1ie().set_bit()),
            }
        );
    }

    pub fn enable_capture_compare_dma_request(&mut self, channel: TimerChannel) {
        // LL_TIM_EnableDMAReq_CC1
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1de().set_bit()),
            }
        );
    }

    pub fn enable_capture_compare_channel(&mut self, channel: TimerChannel) {
        // LL_TIM_CC_EnableChannel
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| w.cc1e().set_bit()),
            }
        );
    }

    pub fn disable_capture_compare_channel(&mut self, channel: TimerChannel) {
        // LL_TIM_CC_EnableChannel
        do_with_timer!(
            self,
            timer,
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| w.cc1e().clear_bit()),
            }
        );
    }

    pub fn enable_counter(&mut self) {
        // LL_TIM_EnableCounter
        do_with_timer!(self, timer, timer.cr1.modify(|_, w| w.cen().set_bit()));
    }

    pub fn disable_counter(&mut self) {
        // LL_TIM_DisableCounter
        do_with_timer!(self, timer, timer.cr1.modify(|_, w| w.cen().clear_bit()));
    }

    pub fn enable_all_outputs(&mut self) {
        // LL_TIM_EnableAllOutputs
        do_with_timer_16_17!(self, timer, timer.bdtr.modify(|_, w| w.moe().set_bit()));
    }

    pub fn listen(&mut self, event: TimerEvent) {
        match event {
            TimerEvent::Active => {
                // LL_TIM_EnableIT_UPDATE
                do_with_timer!(self, timer, timer.dier.modify(|_, w| w.uie().set_bit()))
            }
        }
    }

    pub fn is_pending(&self, event: TimerEvent) -> bool {
        match event {
            TimerEvent::Active => {
                // LL_TIM_IsActiveFlag_UPDATE
                do_with_timer!(self, timer, return timer.sr.read().uif().bit_is_set())
            }
        }
    }

    pub fn unpend(&mut self, event: TimerEvent) {
        match event {
            TimerEvent::Active => {
                // LL_TIM_ClearFlag_UPDATE
                do_with_timer!(
                    self,
                    timer,
                    return timer.sr.modify(|_, w| w.uif().clear_bit())
                )
            }
        }
    }

    pub fn calculate_auto_reload(
        timer_clock_frequency: Hertz,
        prescaler: u32,
        frequency: Hertz,
    ) -> u32 {
        return if (timer_clock_frequency.to_hertz() / (prescaler + 1)) >= frequency.to_hertz() {
            (timer_clock_frequency.to_hertz() / (frequency.to_hertz() * (prescaler + 1))) - 1
        } else {
            0
        };
    }

    pub fn calculate_delay(sys_clk: Hertz, signal_timer_prescaler: u32, unwrap: u16) -> u32 {
        todo!()
    }
}
