use stm32g0::stm32g031::{self};

use super::{hertz::Hertz, rcc::RCC};

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

pub enum TimerChannel {
    Channel1,
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

pub struct Timer {
    register_block: *const stm32g0::stm32g031::tim2::RegisterBlock,
    number: u8,
}

impl Timer {
    pub fn new(_timer: stm32g031::TIM3, rcc: &mut RCC) -> Self {
        rcc.enable_timer3();
        return Self {
            number: 3,
            register_block: stm32g0::stm32g031::TIM3::ptr(),
        };
    }

    pub fn get_timer_number(&self) -> u8 {
        return self.number;
    }

    pub fn disable_auto_reload_preload(&mut self) {
        // LL_TIM_DisableARRPreload
        let timer = unsafe { &*self.register_block };
        timer.cr1.modify(|_, w| w.arpe().clear_bit());
    }

    pub fn set_trigger_output(&mut self, o: TimerTriggerOutput) {
        // LL_TIM_SetTriggerOutput
        let timer = unsafe { &*self.register_block };
        timer.cr2.modify(|_, w| unsafe { w.mms().bits(o.val()) });
    }

    pub fn disable_master_slave_mode(&mut self) {
        // LL_TIM_DisableMasterSlaveMode
        let timer = unsafe { &*self.register_block };
        timer.smcr.modify(|_, w| w.msm().clear_bit());
    }

    pub fn ic_set_active_input(&mut self, channel: TimerChannel, input: TimerActiveInput) {
        // LL_TIM_IC_SetActiveInput
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer
                .ccmr1_input()
                .modify(|_, w| unsafe { w.cc1s().bits(input.val()) }),
        }
    }

    pub fn ic_set_prescaler(&mut self, channel: TimerChannel, prescaler: TimerPrescaler) {
        // LL_TIM_IC_SetPrescaler
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer
                .ccmr1_input()
                .modify(|_, w| unsafe { w.ic1psc().bits(prescaler.val()) }),
        }
    }

    pub fn ic_set_filter(&mut self, channel: TimerChannel, filter: TimerFilter) {
        // LL_TIM_IC_SetFilter
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer
                .ccmr1_input()
                .modify(|_, w| unsafe { w.ic1f().bits(filter.val()) }),
        }
    }

    pub fn ic_set_polarity(&mut self, channel: TimerChannel, polarity: TimerPolarity) {
        // LL_TIM_IC_SetPolarity
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer.ccer.modify(|_, w| match polarity {
                TimerPolarity::BothEdges => w.cc1np().set_bit().cc1p().set_bit(),
            }),
        }
    }

    pub fn set_counter_direction(&mut self, dir: TimerCounterDirection) {
        let timer = unsafe { &*self.register_block };
        timer
            .cr1
            .modify(|_, w| w.dir().bit(dir == TimerCounterDirection::Down));
    }

    pub fn set_center_align_mode(&mut self, mode: TimerCenterAlignMode) {
        let timer = unsafe { &*self.register_block };
        timer.cr1.modify(|_, w| unsafe { w.cms().bits(mode.val()) });
    }

    pub fn set_clock_division(&mut self, div: TimerClockDivision) {
        let timer = unsafe { &*self.register_block };
        timer.cr1.modify(|_, w| unsafe { w.ckd().bits(div.val()) });
    }

    pub fn set_auto_reload(&mut self, reload: u32) {
        // LL_TIM_SetAutoReload
        let timer = unsafe { &*self.register_block };
        timer.arr.modify(|_, w| unsafe { w.bits(reload) });
    }

    pub fn set_prescaler(&mut self, prescaler: u32) {
        // LL_TIM_SetPrescaler
        let timer = unsafe { &*self.register_block };
        timer.psc.modify(|_, w| unsafe { w.bits(prescaler) });
    }

    pub fn generate_event_update(&mut self) {
        // LL_TIM_GenerateEvent_UPDATE
        let timer = unsafe { &*self.register_block };
        timer.egr.write(|w| w.ug().set_bit());
    }

    pub fn set_prescaler_hertz(&mut self, hertz: Hertz, rcc: &RCC) {
        // __LL_TIM_CALC_PSC
        let sys_clk = rcc.get_system_core_clock().to_hertz();
        let clk = hertz.to_hertz();
        let prescaler = if sys_clk >= clk {
            (sys_clk / clk) - 1
        } else {
            0
        };
        self.set_prescaler(prescaler);
    }

    pub fn get_capture_compare_register_address(&self) -> u32 {
        let timer = unsafe { &*self.register_block };
        return &timer.ccr1 as *const _ as u32;
    }

    pub fn enable_capture_compare_interrupt(&mut self, channel: TimerChannel) {
        // LL_TIM_EnableIT_CC1
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1ie().set_bit()),
        }
    }

    pub fn enable_capture_compare_dma_request(&mut self, channel: TimerChannel) {
        // LL_TIM_EnableDMAReq_CC1
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer.dier.modify(|_, w| w.cc1de().set_bit()),
        }
    }

    pub fn enable_capture_compare_channel(&mut self, channel: TimerChannel) {
        // LL_TIM_CC_EnableChannel
        let timer = unsafe { &*self.register_block };
        match channel {
            TimerChannel::Channel1 => timer.ccer.modify(|_, w| w.cc1e().set_bit()),
        }
    }

    pub fn enable_counter(&mut self) {
        // LL_TIM_EnableCounter
        let timer = unsafe { &*self.register_block };
        timer.cr1.modify(|_, w| w.cen().set_bit());
    }
}
