use num_derive::FromPrimitive;
use serde::{Deserialize, Serialize};

#[derive(Debug, PartialEq, Serialize, Deserialize, FromPrimitive, Clone, Copy)]
#[repr(u8)]
pub enum Protocol {
    Unknown = 0,
    Denon = 1,
    Sharp = 2,
    Apple = 3,
    NEC = 4,
    Onkyo = 5,
}

impl Protocol {
    pub fn to_u8(&self) -> u8 {
        return *self as u8;
    }
}
