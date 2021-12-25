#[allow(dead_code)]
pub enum IrPolarity {
    Inverted,
    NotInverted,
}

pub enum IrModulationEnvelopeSignal {
    TIM16 = 0b00,
}

impl IrModulationEnvelopeSignal {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub struct SYSCFG {
    syscfg: stm32g0::stm32g031::SYSCFG,
}

impl SYSCFG {
    pub fn new(syscfg: stm32g0::stm32g031::SYSCFG) -> Self {
        return Self { syscfg };
    }

    pub fn set_ir_modulation_envelope_signal(&mut self, signal: IrModulationEnvelopeSignal) {
        // LL_SYSCFG_SetIRModEnvelopeSignal
        self.syscfg
            .cfgr1
            .modify(|_, w| unsafe { w.ir_mod().bits(signal.val()) });
    }

    pub fn set_ir_polarity(&mut self, ir_polarity: IrPolarity) {
        // LL_SYSCFG_SetIRPolarity
        self.syscfg.cfgr1.modify(|_, w| match ir_polarity {
            IrPolarity::Inverted => w.ir_pol().set_bit(),
            IrPolarity::NotInverted => w.ir_pol().clear_bit(),
        });
    }
}
