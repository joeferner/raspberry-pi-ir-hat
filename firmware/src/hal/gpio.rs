pub trait OutputPin {
    fn is_output_high(&self) -> bool;
    fn toggle(&mut self);
}

pub struct Pin {
    register_block: *const stm32g0::stm32g031::gpioa::RegisterBlock,
    pin: u32,
}

impl OutputPin for Pin {
    fn is_output_high(&self) -> bool {
        let v = unsafe { (*self.register_block).odr.read() }.bits();
        return (v >> self.pin) == 1;
    }

    fn toggle(&mut self) {
        let mut bits = 1 << self.pin;
        if self.is_output_high() {
            bits = bits << 16;
        }
        unsafe {
            (*self.register_block).bsrr.write(|w| w.bits(bits));
        }
    }
}

macro_rules! gpio {
    ($GPIOX:ident, $gpiox:ident, $rcc_enable:ident, $PXx:ident, $Pxn:expr, [
        $(($PXi:ident, $pxi:ident, $i:expr),)+
    ]) => {
        /// GPIO
        pub mod $gpiox {
            use super::super::rcc::RCC;
            use super::Pin;

            /// GPIO parts
            pub struct Parts {
                $(
                    pub $pxi: Pin,
                )+
            }

            pub struct $GPIOX {
                _gpio: stm32g0::stm32g031::$GPIOX
            }

            impl $GPIOX {
                pub fn new(gpio: stm32g0::stm32g031::$GPIOX) -> Self {
                    return Self { _gpio: gpio };
                }

                pub fn split(self, rcc: &mut RCC) -> Parts {
                    rcc.$rcc_enable();
                    return Parts {
                        $(
                            $pxi: Pin {
                                register_block: stm32g0::stm32g031::$GPIOX::ptr() as *const stm32g0::stm32g031::gpioa::RegisterBlock,
                                pin: $i
                            },
                        )+
                    };
                }
            }
        }
    }
}

gpio!(
    GPIOA,
    gpioa,
    enable_gpioa,
    PA,
    0,
    [
        (PA0, pa0, 0),
        (PA1, pa1, 1),
        (PA2, pa2, 2),
        (PA3, pa3, 3),
        (PA4, pa4, 4),
        (PA5, pa5, 5),
        (PA6, pa6, 6),
        (PA7, pa7, 7),
        (PA8, pa8, 8),
        (PA9, pa9, 9),
        (PA10, pa10, 10),
        (PA11, pa11, 11),
        (PA12, pa12, 12),
        (PA13, pa13, 13),
        (PA14, pa14, 14),
        (PA15, pa15, 15),
    ]
);

gpio!(
    GPIOB,
    gpiob,
    enable_gpiob,
    PB,
    0,
    [
        (PB0, pb0, 0),
        (PB1, pb1, 1),
        (PB2, pb2, 2),
        (PB3, pb3, 3),
        (PB4, pb4, 4),
        (PB5, pb5, 5),
        (PB6, pb6, 6),
        (PB7, pb7, 7),
        (PB8, pb8, 8),
        (PB9, pb9, 9),
        (PB10, pb10, 10),
        (PB11, pb11, 11),
        (PB12, pb12, 12),
        (PB13, pb13, 13),
        (PB14, pb14, 14),
        (PB15, pb15, 15),
    ]
);
