use stm32g0::stm32g031::{self};

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

macro_rules! timer {
    ($TIM:ident, $tim:ident, $rcc_enable:ident) => {
        pub fn new(_gpio: stm32g0::stm32g031::$TIM, rcc: &mut RCC) -> Self {
            rcc.$rcc_enable();
            return Self {};
        }

        pub fn get_capture_compare_register_address(&self) -> u32 {
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            return &timer.ccr1 as *const _ as u32;
        }

        pub fn set_clock_division(&mut self, div: TimerClockDivision) {
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.cr1.modify(|_, w| unsafe { w.ckd().bits(div.val()) });
        }

        pub fn set_auto_reload(&mut self, reload: u32) {
            // LL_TIM_SetAutoReload
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.arr.modify(|_, w| unsafe { w.bits(reload) });
        }

        pub fn disable_auto_reload_preload(&mut self) {
            // LL_TIM_DisableARRPreload
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.cr1.modify(|_, w| w.arpe().clear_bit());
        }

        pub fn ic_set_active_input(&mut self, channel: TimerChannel, input: TimerActiveInput) {
            // LL_TIM_IC_SetActiveInput
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.cc1s().bits(input.val()) }),
            }
        }

        pub fn ic_set_prescaler(&mut self, channel: TimerChannel, prescaler: TimerPrescaler) {
            // LL_TIM_IC_SetPrescaler
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.ic1psc().bits(prescaler.val()) }),
            }
        }

        pub fn ic_set_filter(&mut self, channel: TimerChannel, filter: TimerFilter) {
            // LL_TIM_IC_SetFilter
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_input()
                    .modify(|_, w| unsafe { w.ic1f().bits(filter.val()) }),
            }
        }

        pub fn ic_set_polarity(&mut self, channel: TimerChannel, polarity: TimerPolarity) {
            // LL_TIM_IC_SetPolarity
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerPolarity::BothEdges => w.cc1np().set_bit().cc1p().set_bit(),
                }),
            }
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

        pub fn set_prescaler(&mut self, prescaler: u32) {
            // LL_TIM_SetPrescaler
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.psc.modify(|_, w| unsafe { w.bits(prescaler) });
        }

        pub fn generate_event_update(&mut self) {
            // LL_TIM_GenerateEvent_UPDATE
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.egr.write(|w| w.ug().set_bit());
        }

        pub fn enable_capture_compare_dma_request(&mut self, channel: TimerChannel) {
            // LL_TIM_EnableDMAReq_CC1
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1de().set_bit()),
            }
        }

        pub fn enable_capture_compare_channel(&mut self, channel: TimerChannel) {
            // LL_TIM_CC_EnableChannel
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| w.cc1e().set_bit()),
            }
        }

        pub fn enable_counter(&mut self) {
            // LL_TIM_EnableCounter
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.cr1.modify(|_, w| w.cen().set_bit());
        }

        pub fn oc_disable_preload(&mut self, channel: TimerChannel) {
            // LL_TIM_OC_DisablePreload
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1pe().clear_bit()),
            }
        }

        pub fn oc_set_mode(&mut self, channel: TimerChannel, mode: TimerOcMode) {
            // LL_TIM_OC_SetMode
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer
                    .ccmr1_output()
                    .modify(|_, w| w.oc1m().bits(mode.val())),
            }
        }

        pub fn disable_capture_compare_channel(&mut self, channel: TimerChannel) {
            // LL_TIM_CC_EnableChannel
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| w.cc1e().clear_bit()),
            }
        }

        pub fn oc_set_compare(&mut self, channel: TimerChannel, value: u32) {
            // LL_TIM_OC_SetCompareCH1
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccr1.modify(|_, w| unsafe { w.bits(value) }),
            }
        }

        pub fn oc_set_polarity(&mut self, channel: TimerChannel, polarity: TimerOcPolarity) {
            // LL_TIM_OC_SetPolarity
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerOcPolarity::High => w.cc1p().clear_bit(),
                }),
            }
        }

        pub fn oc_set_npolarity(&mut self, channel: TimerChannel, polarity: TimerOcPolarity) {
            // LL_TIM_OC_SetPolarity
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                    TimerOcPolarity::High => w.cc1np().clear_bit(),
                }),
            }
        }

        pub fn enable_capture_compare_interrupt(&mut self, channel: TimerChannel) {
            // LL_TIM_EnableIT_CC1
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1ie().set_bit()),
            }
        }

        pub fn disable_counter(&mut self) {
            // LL_TIM_DisableCounter
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.cr1.modify(|_, w| w.cen().clear_bit());
        }

        pub fn listen(&mut self, event: TimerEvent) {
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match event {
                TimerEvent::Active => {
                    // LL_TIM_EnableIT_UPDATE
                    timer.dier.modify(|_, w| w.uie().set_bit());
                }
            }
        }

        pub fn is_pending(&self, event: TimerEvent) -> bool {
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match event {
                TimerEvent::Active => {
                    // LL_TIM_IsActiveFlag_UPDATE
                    return timer.sr.read().uif().bit_is_set();
                }
            }
        }

        pub fn unpend(&mut self, event: TimerEvent) {
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match event {
                TimerEvent::Active => {
                    // LL_TIM_ClearFlag_UPDATE
                    return timer.sr.modify(|_, w| w.uif().clear_bit());
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
    };
}

macro_rules! timer_16_17 {
    ($TIM:ident) => {
        pub fn set_repetition_counter(&mut self, count: u32) {
            // LL_TIM_SetRepetitionCounter
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.rcr.modify(|_, w| unsafe { w.bits(count) });
        }

        pub fn oc_set_idle_state(&mut self, channel: TimerChannel, idle_state: TimerOcIdleState) {
            // LL_TIM_OC_SetIdleState
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.cr2.modify(|_, w| match idle_state {
                    TimerOcIdleState::Low => w.ois1().clear_bit(),
                }),
            }
        }

        pub fn oc_set_nidle_state(&mut self, channel: TimerChannel, idle_state: TimerOcIdleState) {
            // LL_TIM_OC_SetIdleState
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.cr2.modify(|_, w| match idle_state {
                    TimerOcIdleState::Low => w.ois1n().clear_bit(),
                }),
            }
        }

        pub fn oc_disable_fast_mode(&mut self, channel: TimerChannel) {
            // LL_TIM_OC_DisableFast
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1fe().clear_bit()),
            }
        }

        pub fn oc_enable_preload(&mut self, channel: TimerChannel) {
            // LL_TIM_OC_EnablePreload
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            match channel {
                TimerChannel::Channel1 => timer.ccmr1_output().modify(|_, w| w.oc1pe().set_bit()),
            }
        }

        pub fn enable_all_outputs(&mut self) {
            // LL_TIM_EnableAllOutputs
            let timer = unsafe { &*stm32g0::stm32g031::$TIM::ptr() };
            timer.bdtr.modify(|_, w| w.moe().set_bit());
        }
    };
}

pub mod tim3 {
    use crate::hal::timer::{
        TimerActiveInput, TimerCenterAlignMode, TimerChannel, TimerClockDivision,
        TimerCounterDirection, TimerEvent, TimerFilter, TimerOcMode, TimerOcPolarity,
        TimerPolarity, TimerPrescaler, TimerTriggerOutput,
    };
    use crate::hal::{hertz::Hertz, rcc::RCC, SYS_CLK};

    pub struct TIM3 {}

    impl TIM3 {
        timer!(TIM3, tim3, enable_timer3);

        pub fn set_counter_direction(&mut self, dir: TimerCounterDirection) {
            // LL_TIM_SetCounterMode
            let timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
            timer
                .cr1
                .modify(|_, w| w.dir().bit(dir == TimerCounterDirection::Down));
        }

        pub fn set_center_align_mode(&mut self, mode: TimerCenterAlignMode) {
            let timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
            timer.cr1.modify(|_, w| unsafe { w.cms().bits(mode.val()) });
        }

        pub fn set_trigger_output(&mut self, o: TimerTriggerOutput) {
            // LL_TIM_SetTriggerOutput
            let timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
            timer.cr2.modify(|_, w| unsafe { w.mms().bits(o.val()) });
        }

        pub fn disable_master_slave_mode(&mut self) {
            // LL_TIM_DisableMasterSlaveMode
            let timer = unsafe { &*stm32g0::stm32g031::TIM3::ptr() };
            timer.smcr.modify(|_, w| w.msm().clear_bit());
        }
    }
}

pub mod tim16 {
    use crate::hal::timer::{
        TimerActiveInput, TimerChannel, TimerClockDivision, TimerEvent, TimerFilter,
        TimerOcIdleState, TimerOcMode, TimerOcPolarity, TimerPolarity, TimerPrescaler,
    };
    use crate::hal::{hertz::Hertz, rcc::RCC, SYS_CLK};

    pub struct TIM16 {}

    impl TIM16 {
        timer!(TIM16, tim16, enable_timer16);
        timer_16_17!(TIM16);
    }
}

pub mod tim17 {
    use crate::hal::timer::{
        TimerActiveInput, TimerChannel, TimerClockDivision, TimerEvent, TimerFilter,
        TimerOcIdleState, TimerOcMode, TimerOcPolarity, TimerPolarity, TimerPrescaler,
    };
    use crate::hal::{hertz::Hertz, rcc::RCC, SYS_CLK};

    pub struct TIM17 {}

    impl TIM17 {
        timer!(TIM17, tim17, enable_timer17);
        timer_16_17!(TIM17);
    }
}
