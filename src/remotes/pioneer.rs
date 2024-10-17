use std::time::Duration;

use anyhow::{anyhow, Result};

use crate::lirc::{lirc_writer::LircWriter, LircEvent, LircProtocol};

use super::{send_scan_codes, DecodeResult, Key, Remote};

pub struct PioneerRemote {}

impl PioneerRemote {
    pub fn new() -> Self {
        PioneerRemote {}
    }
}

impl Remote for PioneerRemote {
    fn get_protocol(&self) -> LircProtocol {
        LircProtocol::Nec
    }

    fn get_repeat_count(&self) -> u32 {
        2
    }

    fn get_tx_scan_code_gap(&self) -> Duration {
        Duration::from_millis(25)
    }

    fn get_tx_repeat_gap(&self) -> Duration {
        Duration::from_millis(100)
    }

    fn get_rx_repeat_gap_max(&self) -> Duration {
        Duration::from_millis(200)
    }

    fn get_display_name(&self) -> &str {
        "pioneer"
    }

    fn send(&self, writer: &mut LircWriter, key: Key) -> Result<()> {
        let scan_codes = match key {
            Key::Num0 => vec![0xaa00],
            Key::Num1 => vec![0xaa01],
            Key::Num2 => vec![0xaa02],
            Key::Num3 => vec![0xaa03],
            Key::Num4 => vec![0xaa04],
            Key::Num5 => vec![0xaa05],
            Key::Num6 => vec![0xaa06],
            Key::Num7 => vec![0xaa07],
            Key::Num8 => vec![0xaa08],
            Key::Num9 => vec![0xaa09],
            Key::VolumeUp => vec![0xaa0a],
            Key::VolumeDown => vec![0xaa0b],
            Key::ChannelUp => vec![0xaa10],
            Key::ChannelDown => vec![0xaa11],
            Key::PowerOn => vec![0xaa1a],
            Key::PowerOff => vec![0xaa1b],
            Key::PowerToggle => vec![0xaa1c],
            Key::DirRight => vec![0xaa24],
            Key::DirLeft => vec![0xaa25],
            Key::DirUp => vec![0xaa26],
            Key::DirDown => vec![0xaa27],
            Key::Select => vec![0xaa28],
            Key::Record => vec![0xaa38],
            Key::InputAnt => vec![0xaa45],
            Key::Mute => vec![0xaa49],
            Key::Display => vec![0xaa4a],

            Key::Dot => vec![0xaa5a, 0xaf64],
            Key::ChannelEnter => vec![0xaa5a, 0xaf61],
            Key::ChannelReturn => vec![0xaa5a, 0xaf62],
            Key::InputPc => vec![0xaa5a, 0xaf74],
            Key::Input1 => vec![0xaa5a, 0xaf7a],
            Key::Input2 => vec![0xaa5a, 0xaf7b],
            Key::Input3 => vec![0xaa5a, 0xaf7c],
            Key::Input4 => vec![0xaa5a, 0xaf7d],
            Key::Input5 => vec![0xaa5a, 0xaf7e],
            Key::Input6 => vec![0xaa5a, 0xaf7f],

            Key::Home => vec![0xaa5b, 0xaf20],
            Key::Back => vec![0xaa5b, 0xaf22],
            Key::Guide => vec![0xaa5b, 0xaf24],
            Key::Info => vec![0xaa5b, 0xaf25],
            Key::Menu => vec![0xaa5b, 0xaf27],
            Key::DayPlus => vec![0xaa5b, 0xaf2a],
            Key::DayMinus => vec![0xaa5b, 0xaf2b],
            Key::FavoriteB => vec![0xaa5b, 0xaf2c],
            Key::FavoriteC => vec![0xaa5b, 0xaf2d],
            Key::FavoriteD => vec![0xaa5b, 0xaf2e],
            Key::FavoriteA => vec![0xaa5b, 0xaf2f],
            Key::PageUp => vec![0xaa5b, 0xaf33],
            Key::PageDown => vec![0xaa5b, 0xaf34],

            Key::Size => vec![0xaa5e, 0xaf3a],
            Key::AvSelection => vec![0xaa5e, 0xaf61],
            Key::Sleep => vec![0xaa5e, 0xaf70],

            _ => {
                return Err(anyhow!("unhandled key {:?}", key));
            }
        };
        send_scan_codes(writer, self, scan_codes)
    }

    fn decode(&self, events: &[LircEvent]) -> Option<DecodeResult> {
        if let Some(first_event) = events.first() {
            match first_event.scan_code {
                0xaa00 => return DecodeResult::new(self, Key::Num0),
                0xaa01 => return DecodeResult::new(self, Key::Num1),
                0xaa02 => return DecodeResult::new(self, Key::Num2),
                0xaa03 => return DecodeResult::new(self, Key::Num3),
                0xaa04 => return DecodeResult::new(self, Key::Num4),
                0xaa05 => return DecodeResult::new(self, Key::Num5),
                0xaa06 => return DecodeResult::new(self, Key::Num6),
                0xaa07 => return DecodeResult::new(self, Key::Num7),
                0xaa08 => return DecodeResult::new(self, Key::Num8),
                0xaa09 => return DecodeResult::new(self, Key::Num9),
                0xaa0a => return DecodeResult::new(self, Key::VolumeUp),
                0xaa0b => return DecodeResult::new(self, Key::VolumeDown),
                0xaa10 => return DecodeResult::new(self, Key::ChannelUp),
                0xaa11 => return DecodeResult::new(self, Key::ChannelDown),
                0xaa1a => return DecodeResult::new(self, Key::PowerOn),
                0xaa1b => return DecodeResult::new(self, Key::PowerOff),
                0xaa1c => return DecodeResult::new(self, Key::PowerToggle),
                0xaa24 => return DecodeResult::new(self, Key::DirRight),
                0xaa25 => return DecodeResult::new(self, Key::DirLeft),
                0xaa26 => return DecodeResult::new(self, Key::DirUp),
                0xaa27 => return DecodeResult::new(self, Key::DirDown),
                0xaa28 => return DecodeResult::new(self, Key::Select),
                0xaa38 => return DecodeResult::new(self, Key::Record),
                0xaa45 => return DecodeResult::new(self, Key::InputAnt),
                0xaa49 => return DecodeResult::new(self, Key::Mute),
                0xaa4a => return DecodeResult::new(self, Key::Display),
                0xaa5a => {
                    if let Some(second_event) = events.get(1) {
                        match second_event.scan_code {
                            0xaf64 => return DecodeResult::new(self, Key::Dot),
                            0xaf61 => return DecodeResult::new(self, Key::ChannelEnter),
                            0xaf62 => return DecodeResult::new(self, Key::ChannelReturn),
                            0xaf74 => return DecodeResult::new(self, Key::InputPc),
                            0xaf7a => return DecodeResult::new(self, Key::Input1),
                            0xaf7b => return DecodeResult::new(self, Key::Input2),
                            0xaf7c => return DecodeResult::new(self, Key::Input3),
                            0xaf7d => return DecodeResult::new(self, Key::Input4),
                            0xaf7e => return DecodeResult::new(self, Key::Input5),
                            0xaf7f => return DecodeResult::new(self, Key::Input6),
                            _ => return None,
                        }
                    }
                }
                0xaa5b => {
                    if let Some(second_event) = events.get(1) {
                        match second_event.scan_code {
                            0xaf20 => return DecodeResult::new(self, Key::Home),
                            0xaf22 => return DecodeResult::new(self, Key::Back),
                            0xaf24 => return DecodeResult::new(self, Key::Guide),
                            0xaf25 => return DecodeResult::new(self, Key::Info),
                            0xaf27 => return DecodeResult::new(self, Key::Menu),
                            0xaf2a => return DecodeResult::new(self, Key::DayPlus),
                            0xaf2b => return DecodeResult::new(self, Key::DayMinus),
                            0xaf2c => return DecodeResult::new(self, Key::FavoriteB),
                            0xaf2d => return DecodeResult::new(self, Key::FavoriteC),
                            0xaf2e => return DecodeResult::new(self, Key::FavoriteD),
                            0xaf2f => return DecodeResult::new(self, Key::FavoriteA),
                            0xaf33 => return DecodeResult::new(self, Key::PageUp),
                            0xaf34 => return DecodeResult::new(self, Key::PageDown),
                            _ => return None,
                        }
                    }
                }
                0xaa5e => {
                    if let Some(second_event) = events.get(1) {
                        match second_event.scan_code {
                            0xaf3a => return DecodeResult::new(self, Key::Size),
                            0xaf61 => return DecodeResult::new(self, Key::AvSelection),
                            0xaf70 => return DecodeResult::new(self, Key::Sleep),
                            _ => return None,
                        }
                    }
                }
                _ => return None,
            }
        }
        None
    }
}
