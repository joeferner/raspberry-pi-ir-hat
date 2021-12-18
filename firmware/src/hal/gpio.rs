#[derive(Debug, PartialEq, Clone, Copy)]
#[repr(u8)]
pub enum PortName {
    A = 1,
    B = 2,
    C = 3,
}

pub struct Parts {
    pub p0: Pin,
    pub p1: Pin,
    pub p2: Pin,
    pub p3: Pin,
    pub p4: Pin,
    pub p5: Pin,
    pub p6: Pin,
    pub p7: Pin,
    pub p8: Pin,
    pub p9: Pin,
    pub p10: Pin,
    pub p11: Pin,
    pub p12: Pin,
    pub p13: Pin,
    pub p14: Pin,
    pub p15: Pin,
}

pub struct Port {
    port_name: PortName,
}

impl Port {
    pub fn split(self) -> Parts {
        return Parts {
            p0: Pin {
                pin: 0,
                port_name: self.port_name,
            },
            p1: Pin {
                pin: 1,
                port_name: self.port_name,
            },
            p2: Pin {
                pin: 2,
                port_name: self.port_name,
            },
            p3: Pin {
                pin: 3,
                port_name: self.port_name,
            },
            p4: Pin {
                pin: 4,
                port_name: self.port_name,
            },
            p5: Pin {
                pin: 5,
                port_name: self.port_name,
            },
            p6: Pin {
                pin: 6,
                port_name: self.port_name,
            },
            p7: Pin {
                pin: 7,
                port_name: self.port_name,
            },
            p8: Pin {
                pin: 8,
                port_name: self.port_name,
            },
            p9: Pin {
                pin: 9,
                port_name: self.port_name,
            },
            p10: Pin {
                pin: 10,
                port_name: self.port_name,
            },
            p11: Pin {
                pin: 11,
                port_name: self.port_name,
            },
            p12: Pin {
                pin: 12,
                port_name: self.port_name,
            },
            p13: Pin {
                pin: 13,
                port_name: self.port_name,
            },
            p14: Pin {
                pin: 14,
                port_name: self.port_name,
            },
            p15: Pin {
                pin: 15,
                port_name: self.port_name,
            },
        };
    }
}

pub trait OutputPin {
    fn is_output_high(&self) -> bool;
    fn toggle(&mut self);
    fn set_value(&mut self, value: bool);
}

pub enum Mode {
    Input = 0b00,
    Output = 0b01,
    AlternateFunction = 0b10,
    Analog = 0b11,
}

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

pub enum OutputType {
    PushPull,
    OpenDrain,
}

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

macro_rules! do_with_port {
    ($self:ident, $gpio:ident, $action:expr) => {
        match $self.port_name {
            PortName::A => {
                let $gpio = unsafe { &*stm32g0::stm32g031::GPIOA::ptr() };
                $action;
            }
            PortName::B => {
                let $gpio = unsafe { &*stm32g0::stm32g031::GPIOB::ptr() };
                $action;
            }
            PortName::C => {
                let $gpio = unsafe { &*stm32g0::stm32g031::GPIOC::ptr() };
                $action;
            }
        }
    };
}

pub struct Pin {
    pin: u8,
    port_name: PortName,
}

impl Pin {
    pub fn get_pin(&self) -> u8 {
        return self.pin;
    }

    pub fn get_port_name(&self) -> PortName {
        return self.port_name;
    }

    pub fn set_as_output(&mut self) {
        self.set_mode(Mode::Output);
    }

    pub fn set_as_input(&mut self) {
        self.set_mode(Mode::Input);
    }

    pub fn set_as_alternate_function(&mut self, mode: AlternateFunctionMode) {
        self.set_mode(Mode::AlternateFunction);

        do_with_port!(self, gpio, unsafe {
            match self.pin {
                0 => gpio.afrl.modify(|_, w| w.afsel0().bits(mode.val())),
                1 => gpio.afrl.modify(|_, w| w.afsel1().bits(mode.val())),
                2 => gpio.afrl.modify(|_, w| w.afsel2().bits(mode.val())),
                3 => gpio.afrl.modify(|_, w| w.afsel3().bits(mode.val())),
                4 => gpio.afrl.modify(|_, w| w.afsel4().bits(mode.val())),
                5 => gpio.afrl.modify(|_, w| w.afsel5().bits(mode.val())),
                6 => gpio.afrl.modify(|_, w| w.afsel6().bits(mode.val())),
                7 => gpio.afrl.modify(|_, w| w.afsel7().bits(mode.val())),
                8 => gpio.afrh.modify(|_, w| w.afsel8().bits(mode.val())),
                9 => gpio.afrh.modify(|_, w| w.afsel9().bits(mode.val())),
                10 => gpio.afrh.modify(|_, w| w.afsel10().bits(mode.val())),
                11 => gpio.afrh.modify(|_, w| w.afsel11().bits(mode.val())),
                12 => gpio.afrh.modify(|_, w| w.afsel12().bits(mode.val())),
                13 => gpio.afrh.modify(|_, w| w.afsel13().bits(mode.val())),
                14 => gpio.afrh.modify(|_, w| w.afsel14().bits(mode.val())),
                15 => gpio.afrh.modify(|_, w| w.afsel15().bits(mode.val())),
                _ => panic!(),
            }
        });
    }

    pub fn set_mode(&mut self, mode: Mode) {
        do_with_port!(self, gpio, unsafe {
            unsafe {
                gpio.moder.modify(|r, w| {
                    let mut v = r.bits();
                    v = v & !(0b11u32 << (self.pin * 2));
                    v = v | ((mode as u32) << (self.pin * 2));
                    return w.bits(v);
                });
            }
        })
    }

    pub fn set_output_type_push_pull(&mut self) {
        self.set_output_type(OutputType::PushPull);
    }

    pub fn set_output_type_open_drain(&mut self) {
        self.set_output_type(OutputType::OpenDrain);
    }

    pub fn set_output_type(&mut self, output_type: OutputType) {
        let v = match output_type {
            OutputType::OpenDrain => true,
            OutputType::PushPull => false,
        };
        do_with_port!(self, gpio, unsafe {
            match self.pin {
                0 => gpio.otyper.modify(|_, w| w.ot0().bit(v)),
                1 => gpio.otyper.modify(|_, w| w.ot1().bit(v)),
                2 => gpio.otyper.modify(|_, w| w.ot2().bit(v)),
                3 => gpio.otyper.modify(|_, w| w.ot3().bit(v)),
                4 => gpio.otyper.modify(|_, w| w.ot4().bit(v)),
                5 => gpio.otyper.modify(|_, w| w.ot5().bit(v)),
                6 => gpio.otyper.modify(|_, w| w.ot6().bit(v)),
                7 => gpio.otyper.modify(|_, w| w.ot7().bit(v)),
                8 => gpio.otyper.modify(|_, w| w.ot8().bit(v)),
                9 => gpio.otyper.modify(|_, w| w.ot9().bit(v)),
                10 => gpio.otyper.modify(|_, w| w.ot10().bit(v)),
                11 => gpio.otyper.modify(|_, w| w.ot11().bit(v)),
                12 => gpio.otyper.modify(|_, w| w.ot12().bit(v)),
                13 => gpio.otyper.modify(|_, w| w.ot13().bit(v)),
                14 => gpio.otyper.modify(|_, w| w.ot14().bit(v)),
                15 => gpio.otyper.modify(|_, w| w.ot15().bit(v)),
                _ => panic!(),
            }
        });
    }

    pub fn set_pull_none(&mut self) {
        self.set_pull(Pull::None);
    }

    pub fn set_pull(&mut self, pull: Pull) {
        let v = pull.val();
        do_with_port!(self, gpio, unsafe {
            match self.pin {
                0 => gpio.pupdr.modify(|_, w| w.pupdr0().bits(v)),
                1 => gpio.pupdr.modify(|_, w| w.pupdr1().bits(v)),
                2 => gpio.pupdr.modify(|_, w| w.pupdr2().bits(v)),
                3 => gpio.pupdr.modify(|_, w| w.pupdr3().bits(v)),
                4 => gpio.pupdr.modify(|_, w| w.pupdr4().bits(v)),
                5 => gpio.pupdr.modify(|_, w| w.pupdr5().bits(v)),
                6 => gpio.pupdr.modify(|_, w| w.pupdr6().bits(v)),
                7 => gpio.pupdr.modify(|_, w| w.pupdr7().bits(v)),
                8 => gpio.pupdr.modify(|_, w| w.pupdr8().bits(v)),
                9 => gpio.pupdr.modify(|_, w| w.pupdr9().bits(v)),
                10 => gpio.pupdr.modify(|_, w| w.pupdr10().bits(v)),
                11 => gpio.pupdr.modify(|_, w| w.pupdr11().bits(v)),
                12 => gpio.pupdr.modify(|_, w| w.pupdr12().bits(v)),
                13 => gpio.pupdr.modify(|_, w| w.pupdr13().bits(v)),
                14 => gpio.pupdr.modify(|_, w| w.pupdr14().bits(v)),
                15 => gpio.pupdr.modify(|_, w| w.pupdr15().bits(v)),
                _ => panic!(),
            }
        })
    }

    pub fn set_speed_low(&mut self) {
        self.set_speed(Speed::Low);
    }

    pub fn set_speed(&self, speed: Speed) {
        let v = speed.val();
        do_with_port!(self, gpio, unsafe {
            match self.pin {
                0 => gpio.ospeedr.modify(|_, w| w.ospeedr0().bits(v)),
                1 => gpio.ospeedr.modify(|_, w| w.ospeedr1().bits(v)),
                2 => gpio.ospeedr.modify(|_, w| w.ospeedr2().bits(v)),
                3 => gpio.ospeedr.modify(|_, w| w.ospeedr3().bits(v)),
                4 => gpio.ospeedr.modify(|_, w| w.ospeedr4().bits(v)),
                5 => gpio.ospeedr.modify(|_, w| w.ospeedr5().bits(v)),
                6 => gpio.ospeedr.modify(|_, w| w.ospeedr6().bits(v)),
                7 => gpio.ospeedr.modify(|_, w| w.ospeedr7().bits(v)),
                8 => gpio.ospeedr.modify(|_, w| w.ospeedr8().bits(v)),
                9 => gpio.ospeedr.modify(|_, w| w.ospeedr9().bits(v)),
                10 => gpio.ospeedr.modify(|_, w| w.ospeedr10().bits(v)),
                11 => gpio.ospeedr.modify(|_, w| w.ospeedr11().bits(v)),
                12 => gpio.ospeedr.modify(|_, w| w.ospeedr12().bits(v)),
                13 => gpio.ospeedr.modify(|_, w| w.ospeedr13().bits(v)),
                14 => gpio.ospeedr.modify(|_, w| w.ospeedr14().bits(v)),
                15 => gpio.ospeedr.modify(|_, w| w.ospeedr15().bits(v)),
                _ => panic!(),
            }
        })
    }
}

impl OutputPin for Pin {
    fn is_output_high(&self) -> bool {
        do_with_port!(self, gpio, {
            let v = gpio.odr.read().bits();
            return ((v >> self.pin) & 0b1) == 1;
        });
    }

    fn toggle(&mut self) {
        let mut bits = 1u32 << self.pin;
        if self.is_output_high() {
            bits = bits << 16;
        }
        do_with_port!(self, gpio, unsafe {
            gpio.bsrr.write(|w| w.bits(bits));
        })
    }

    fn set_value(&mut self, value: bool) {
        let bits = if value {
            1u32 << self.pin
        } else {
            (1u32 << 16) << self.pin
        };
        do_with_port!(self, gpio, gpio.bsrr.write(|w| unsafe { w.bits(bits) }));
    }
}

macro_rules! gpio {
    ($GPIOX:ident, $gpiox:ident, $port_name:ident, $rcc_enable:ident) => {
        pub mod $gpiox {
            use super::Port;
            use super::PortName;
            use crate::hal::rcc::RCC;

            pub fn new(_gpio: stm32g0::stm32g031::$GPIOX, rcc: &mut RCC) -> Port {
                rcc.$rcc_enable();
                rcc.$rcc_enable();
                return Port {
                    port_name: PortName::$port_name,
                };
            }
        }
    };
}

gpio!(GPIOA, gpioa, A, enable_gpioa);
gpio!(GPIOB, gpiob, B, enable_gpiob);
gpio!(GPIOC, gpioc, C, enable_gpioc);
