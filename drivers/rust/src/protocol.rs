use num_derive::FromPrimitive;
use serde::{Serialize, Deserialize};

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
