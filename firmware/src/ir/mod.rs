use std::borrow::Borrow;

use crate::duration::Duration;

mod denon;
mod utils;

pub const MARK_EXCESS_MICROS: u16 = 20;
pub const MICROS_PER_TICK: u16 = 50;
pub const MINIMUM_DEAD_DURATION: Duration = Duration::from_millis(300);

pub enum Protocol {
    Sharp,
    Denon,
}

pub struct IrSignal {
    pub protocol: Protocol,
    pub address: u16,
    pub command: u16,
}

impl IrSignal {
    pub fn new(protocol: Protocol, address: u16, command: u16) -> Self {
        return IrSignal {
            protocol,
            address,
            command,
        };
    }
}

pub struct IrDecoder {
    queue: [u16; 256],
    queue_pos: usize,
    last_decoded_command: u16,
}

impl IrDecoder {
    pub fn new() -> Self {
        return IrDecoder {
            queue: [0; 256],
            queue_pos: 0,
            last_decoded_command: 0,
        };
    }

    pub fn push(&mut self, ir: u16) {
        self.queue[self.queue_pos] = ir;
        self.queue_pos += 1;
    }

    pub fn reset(&mut self) {
        self.queue_pos = 0;
    }

    pub fn decode(&mut self) -> Option<IrSignal> {
        let result = denon::decode(&self.queue[0..self.queue_pos], self.last_decoded_command);
        if result.is_some() {
            self.last_decoded_command = result.as_ref().unwrap().command;
            return result;
        }
        return Option::None;
    }
}

impl IrSignal {
    pub fn to_string(&self) -> &str {
        todo!()
    }
}
