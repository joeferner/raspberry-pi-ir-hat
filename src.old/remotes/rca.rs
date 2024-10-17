use std::time::Duration;

use crate::lirc::{lirc_writer::LircWriter, LircEvent, LircProtocol};

use super::{DecodeResult, Key, Remote};
use crate::my_error::Result;
pub struct RcaRemote {}

impl RcaRemote {
    pub fn new() -> Self {
        return RcaRemote {};
    }
}

impl Remote for RcaRemote {
    fn get_protocol(&self) -> LircProtocol {
        return LircProtocol::Sony12;
    }

    fn get_repeat_count(&self) -> u32 {
        return 3;
    }

    fn get_tx_scan_code_gap(&self) -> Duration {
        return Duration::from_millis(0);
    }

    fn get_tx_repeat_gap(&self) -> Duration {
        return Duration::from_millis(50);
    }

    fn get_rx_repeat_gap_max(&self) -> Duration {
        return Duration::from_millis(200);
    }

    fn get_display_name(&self) -> &str {
        return "rca";
    }

    fn send(&self, _writer: &mut LircWriter, _key: Key) -> Result<()> {
        todo!()
    }

    fn decode(&self, events: &Vec<LircEvent>) -> Option<DecodeResult> {
        if let Option::Some(first_event) = events.get(0) {
            match first_event.scan_code {
                0x10015 => return DecodeResult::new(self, Key::PowerToggle),
                0x10012 => return DecodeResult::new(self, Key::VolumeUp),
                0x10013 => return DecodeResult::new(self, Key::VolumeDown),
                0x10014 => return DecodeResult::new(self, Key::Mute),
                _ => return Option::None,
            }
        }
        return Option::None;
    }
}
