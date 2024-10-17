use std::time::Duration;

use anyhow::Result;

use crate::lirc::{lirc_writer::LircWriter, LircEvent, LircProtocol};

use super::{DecodeResult, Key, Remote};
pub struct RcaRemote {}

impl RcaRemote {
    pub fn new() -> Self {
        RcaRemote {}
    }
}

impl Remote for RcaRemote {
    fn get_protocol(&self) -> LircProtocol {
        LircProtocol::Sony12
    }

    fn get_repeat_count(&self) -> u32 {
        3
    }

    fn get_tx_scan_code_gap(&self) -> Duration {
        Duration::from_millis(0)
    }

    fn get_tx_repeat_gap(&self) -> Duration {
        Duration::from_millis(50)
    }

    fn get_rx_repeat_gap_max(&self) -> Duration {
        Duration::from_millis(200)
    }

    fn get_display_name(&self) -> &str {
        "rca"
    }

    fn send(&self, _writer: &mut LircWriter, _key: Key) -> Result<()> {
        todo!()
    }

    fn decode(&self, events: &[LircEvent]) -> Option<DecodeResult> {
        if let Some(first_event) = events.first() {
            match first_event.scan_code {
                0x10015 => return DecodeResult::new(self, Key::PowerToggle),
                0x10012 => return DecodeResult::new(self, Key::VolumeUp),
                0x10013 => return DecodeResult::new(self, Key::VolumeDown),
                0x10014 => return DecodeResult::new(self, Key::Mute),
                _ => return None,
            }
        }
        None
    }
}
