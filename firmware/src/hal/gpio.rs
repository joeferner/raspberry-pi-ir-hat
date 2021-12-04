use super::rcc::RCC;

pub struct Port {
    register_block: *const stm32g0::stm32g031::gpioa::RegisterBlock,
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

impl Port {
    pub fn split(self, rcc: &mut RCC) -> Parts {
        return Parts {
            p0: Pin {
                register_block: self.register_block,
                pin: 0,
            },
            p1: Pin {
                register_block: self.register_block,
                pin: 1,
            },
            p2: Pin {
                register_block: self.register_block,
                pin: 2,
            },
            p3: Pin {
                register_block: self.register_block,
                pin: 3,
            },
            p4: Pin {
                register_block: self.register_block,
                pin: 4,
            },
            p5: Pin {
                register_block: self.register_block,
                pin: 5,
            },
            p6: Pin {
                register_block: self.register_block,
                pin: 6,
            },
            p7: Pin {
                register_block: self.register_block,
                pin: 7,
            },
            p8: Pin {
                register_block: self.register_block,
                pin: 8,
            },
            p9: Pin {
                register_block: self.register_block,
                pin: 9,
            },
            p10: Pin {
                register_block: self.register_block,
                pin: 10,
            },
            p11: Pin {
                register_block: self.register_block,
                pin: 11,
            },
            p12: Pin {
                register_block: self.register_block,
                pin: 12,
            },
            p13: Pin {
                register_block: self.register_block,
                pin: 13,
            },
            p14: Pin {
                register_block: self.register_block,
                pin: 14,
            },
            p15: Pin {
                register_block: self.register_block,
                pin: 15,
            },
        };
    }
}

pub trait OutputPin {
    fn is_output_high(&self) -> bool;
    fn toggle(&mut self);
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

pub struct Pin {
    register_block: *const stm32g0::stm32g031::gpioa::RegisterBlock,
    pin: u32,
}

impl Pin {
    pub fn set_as_output(&mut self) {
        self.set_mode(Mode::Output);
    }

    pub fn set_as_input(&mut self) {
        self.set_mode(Mode::Input);
    }

    pub fn set_as_alternate_function(&mut self, mode: AlternateFunctionMode) {
        self.set_mode(Mode::AlternateFunction);

        let gpio = unsafe { &*self.register_block };
        unsafe {
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
        }
    }

    pub fn set_mode(&mut self, mode: Mode) {
        unsafe {
            (*self.register_block).moder.modify(|r, w| {
                let mut v = r.bits();
                v = v & !(0b11u32 << (self.pin * 2));
                v = v | ((mode as u32) << (self.pin * 2));
                return w.bits(v);
            });
        }
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
        let gpio = unsafe { &*self.register_block };
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
    }

    pub fn set_pull_none(&mut self) {
        self.set_pull(Pull::None);
    }

    pub fn set_pull(&mut self, pull: Pull) {
        let v = pull.val();
        let gpio = unsafe { &*self.register_block };
        unsafe {
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
        }
    }

    pub fn set_speed_low(&mut self) {
        self.set_speed(Speed::Low);
    }

    pub fn set_speed(&self, speed: Speed) {
        let v = speed.val();
        let gpio = unsafe { &*self.register_block };
        unsafe {
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
        }
    }
}

impl OutputPin for Pin {
    fn is_output_high(&self) -> bool {
        let v = unsafe { (*self.register_block).odr.read() }.bits();
        return ((v >> self.pin) & 0b1) == 1;
    }

    fn toggle(&mut self) {
        let mut bits = 1u32 << self.pin;
        if self.is_output_high() {
            bits = bits << 16;
        }
        unsafe {
            (*self.register_block).bsrr.write(|w| w.bits(bits));
        }
    }
}

macro_rules! gpio {
    ($GPIOX:ident, $gpiox:ident) => {
        pub mod $gpiox {
            use super::Port;

            pub fn new(_gpio: stm32g0::stm32g031::$GPIOX) -> Port {
                return Port {
                    register_block: stm32g0::stm32g031::$GPIOX::ptr()
                        as *const stm32g0::stm32g031::gpioa::RegisterBlock,
                };
            }
        }
    };
}

gpio!(GPIOA, gpioa);
gpio!(GPIOB, gpiob);
