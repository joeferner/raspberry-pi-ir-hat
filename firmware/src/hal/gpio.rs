macro_rules! gpio {
    ($GPIO:ident, $gpio:ident, $rcc_enable:ident, [
        $($PXi:ident: ($pxi:ident, $i:expr, $afr:ident, $afsel:ident, $ot:ident, $ospeedr:ident, $pupdr:ident),)+
    ]) => {
        pub mod $gpio {
            use crate::hal::rcc::RCC;
            use crate::hal::gpio::{Pull, Speed, Mode, OutputType, OutputPin, AlternateFunctionMode};

            pub struct $GPIO {
                $(
                    pub $pxi: $PXi,
                )+
            }

            impl $GPIO {
                pub fn new(_gpio: stm32g0::stm32g031::$GPIO, rcc: &mut RCC) -> Self {
                    rcc.$rcc_enable();
                    return Self {
                        $(
                            $pxi: $PXi {},
                        )+
                    };
                }
            }

            $(
                pub struct $PXi {
                }

                impl $PXi {
                    #[allow(dead_code)]
                    pub fn set_as_output(&mut self) {
                        self.set_mode(Mode::Output);
                    }

                    #[allow(dead_code)]
                    pub fn set_as_input(&mut self) {
                        self.set_mode(Mode::Input);
                    }

                    pub fn set_mode(&mut self, mode: Mode) {
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        gpio.moder.modify(|r, w| unsafe {
                            let mut v = r.bits();
                            v = v & !(0b11u32 << ($i * 2));
                            v = v | ((mode as u32) << ($i * 2));
                            return w.bits(v);
                        });
                    }

                    #[allow(dead_code)]
                    pub fn set_as_alternate_function(&mut self, mode: AlternateFunctionMode) {
                        self.set_mode(Mode::AlternateFunction);

                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        gpio.$afr.modify(|_, w| unsafe { w.$afsel().bits(mode.val()) });
                    }

                    #[allow(dead_code)]
                    pub fn set_output_type_push_pull(&mut self) {
                        self.set_output_type(OutputType::PushPull);
                    }

                    #[allow(dead_code)]
                    pub fn set_output_type_open_drain(&mut self) {
                        self.set_output_type(OutputType::OpenDrain);
                    }

                    #[allow(dead_code)]
                    pub fn set_output_type(&mut self, output_type: OutputType) {
                        let v = match output_type {
                            OutputType::OpenDrain => true,
                            OutputType::PushPull => false,
                        };
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        gpio.otyper.modify(|_, w| w.$ot().bit(v));
                    }

                    #[allow(dead_code)]
                    pub fn set_speed_low(&mut self) {
                        self.set_speed(Speed::Low);
                    }

                    #[allow(dead_code)]
                    pub fn set_speed(&self, speed: Speed) {
                        let v = speed.val();
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        gpio.ospeedr.modify(|_, w| unsafe { w.$ospeedr().bits(v) });
                    }

                    #[allow(dead_code)]
                    pub fn set_pull_none(&mut self) {
                        self.set_pull(Pull::None);
                    }

                    pub fn set_pull(&mut self, pull: Pull) {
                        let v = pull.val();
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        gpio.pupdr.modify(|_, w| unsafe { w.$pupdr().bits(v) });
                    }
                }

                impl OutputPin for $PXi {
                    fn is_output_high(&self) -> bool {
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        let v = gpio.odr.read().bits();
                        return ((v >> $i) & 0b1) == 1;
                    }

                    fn toggle(&mut self) {
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        let mut bits = 1u32 << $i;
                        if self.is_output_high() {
                            bits = bits << 16;
                        }
                        gpio.bsrr.write(|w| unsafe { w.bits(bits) });
                    }

                    fn set_value(&mut self, value: bool) {
                        let gpio = unsafe { &*stm32g0::stm32g031::$GPIO::ptr() };
                        let bits = if value {
                            1u32 << $i
                        } else {
                            (1u32 << 16) << $i
                        };
                        gpio.bsrr.write(|w| unsafe { w.bits(bits) });
                    }
                }
            )+
        }
    };
}

gpio!(GPIOA, gpioa, enable_gpioa, [
    PA6: (pa6, 6, afrl, afsel6, ot6, ospeedr6, pupdr6),
    PA7: (pa7, 7, afrl, afsel7, ot7, ospeedr7, pupdr7),
]);
gpio!(GPIOB, gpiob, enable_gpiob, [
    PB6: (pb6, 6, afrl, afsel6, ot6, ospeedr6, pupdr6),
    PB7: (pb7, 7, afrl, afsel7, ot7, ospeedr7, pupdr7),
]);
gpio!(GPIOC, gpioc, enable_gpioc, [
    PC14: (pc14, 14, afrh, afsel14, ot14, ospeedr14, pupdr14),
]);

pub trait OutputPin {
    fn is_output_high(&self) -> bool;
    fn toggle(&mut self);
    fn set_value(&mut self, value: bool);
}

#[allow(dead_code)]
pub enum Mode {
    Input = 0b00,
    Output = 0b01,
    AlternateFunction = 0b10,
    Analog = 0b11,
}

#[allow(dead_code)]
pub enum AlternateFunctionMode {
    AF0 = 0b0000,
    AF1 = 0b0001,
    AF2 = 0b0010,
    AF3 = 0b0011,
    AF4 = 0b0100,
    AF5 = 0b0101,
    AF6 = 0b0110,
    AF7 = 0b0111,
}

impl AlternateFunctionMode {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

#[allow(dead_code)]
pub enum OutputType {
    PushPull,
    OpenDrain,
}

#[allow(dead_code)]
pub enum Pull {
    None = 0b00,
    PullUp = 0b01,
    PullDown = 0b10,
}

impl Pull {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

#[allow(dead_code)]
pub enum Speed {
    VeryLow = 0b00,
    Low = 0b01,
    High = 0b10,
    VeryHigh = 0b11,
}

impl Speed {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}
