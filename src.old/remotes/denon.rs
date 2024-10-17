use std::time::Duration;

use crate::{
    lirc::{lirc_writer::LircWriter, LircEvent, LircProtocol},
    my_error::MyError,
};

use super::{send_scan_codes, DecodeResult, Key, Remote};
use crate::my_error::Result;
pub struct DenonRemote {}

impl DenonRemote {
    pub fn new() -> Self {
        return DenonRemote {};
    }
}

impl Remote for DenonRemote {
    fn get_protocol(&self) -> LircProtocol {
        return LircProtocol::Sharp;
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
        return "denon";
    }

    fn send(&self, writer: &mut LircWriter, key: Key) -> Result<()> {
        let scan_codes = match key {
            Key::PowerOn => vec![0x02e1],
            Key::PowerOff => vec![0x02e2],
            Key::Dimmer => vec![0x0cdf],
            Key::InputCd => vec![0x02c4],
            Key::InputDbs => vec![0x02c8],
            Key::InputDvd => vec![0x02e3],
            Key::InputMode => vec![0x0cb6],
            Key::InputModeAnalog => vec![0x02dc],
            Key::InputModeExtIn => vec![0x0cb5],
            Key::InputPhono => vec![0x02c3],
            Key::InputTape => vec![0x02d2],
            Key::InputTuner => vec![0x02c5],
            Key::InputTv => vec![0x02c9],
            Key::InputVAux => vec![0x02cc],
            Key::InputVcr1 => vec![0x02cd],
            Key::InputVcr2 => vec![0x02ce],
            Key::InputVdp => vec![0x02ca],
            Key::Mode5ch7ch => vec![0x0ca8],
            Key::ModeCinema => vec![0x0495],
            Key::ModeDspSimu => vec![0x02e6],
            Key::ModeGame => vec![0x0497],
            Key::ModeMusic => vec![0x0496],
            Key::ModePureDirect => vec![0x026a],
            Key::ModeStandard => vec![0x02e4],
            Key::ModeStereo => vec![0x0c9d],
            Key::ModeSurroundBack => vec![0x0c82],
            Key::OnScreen => vec![0x02df],
            Key::PowerOffMain => vec![0x027a],
            Key::PowerOnMain => vec![0x0279],
            Key::PresetNext => vec![0x0cd6],
            Key::PresetPrevious => vec![0x0cd5],
            Key::RoomEq => vec![0x049d],
            Key::Speaker => vec![0x02ed],
            Key::SurroundParameter => vec![0x0ca1],
            Key::SystemSetup => vec![0x0ca0],
            Key::TestTone => vec![0x02ea],
            Key::TunerBand => vec![0x0cd7],
            Key::TunerMemory => vec![0x0ccc],
            Key::TunerMode => vec![0x0cd8],
            Key::TunerShift => vec![0x0ccd],
            Key::VideoOff => vec![0x0cb2],
            Key::VideoSelect => vec![0x02d8],
            Key::VolumeUp => vec![0x02f1],
            Key::VolumeDown => vec![0x02f2],
            Key::ChannelUp => vec![0x0cd9],
            Key::ChannelDown => vec![0x0cda],
            Key::DirUp => vec![0x0ca3],
            Key::DirDown => vec![0x0ca4],
            Key::DirLeft => vec![0x0c7f],
            Key::DirRight => vec![0x02dd],
            Key::Select => vec![0x02e0],
            Key::Mute => vec![0x02f0],
            Key::Num1 => vec![0x0cc1],
            Key::Num2 => vec![0x0cc2],
            Key::Num3 => vec![0x0cc3],
            Key::Num4 => vec![0x0cc4],
            Key::Num5 => vec![0x0cc5],
            Key::Num6 => vec![0x0cc6],
            Key::Num7 => vec![0x0cc7],
            Key::Num8 => vec![0x0cc8],
            Key::Num9 => vec![0x0cc9],
            Key::Num0 => vec![0x0cca],
            _ => {
                return Result::Err(MyError::new(format!("unhandled key {:?}", key)));
            }
        };
        return send_scan_codes(writer, self, scan_codes);
    }

    fn decode(&self, events: &Vec<LircEvent>) -> Option<DecodeResult> {
        if let Option::Some(first_event) = events.get(0) {
            match first_event.scan_code {
                0x02e1 => return DecodeResult::new(self, Key::PowerOn),
                0x02e2 => return DecodeResult::new(self, Key::PowerOff),
                0x0cdf => return DecodeResult::new(self, Key::Dimmer),
                0x02c4 => return DecodeResult::new(self, Key::InputCd),
                0x02c8 => return DecodeResult::new(self, Key::InputDbs),
                0x02e3 => return DecodeResult::new(self, Key::InputDvd),
                0x0cb6 => return DecodeResult::new(self, Key::InputMode),
                0x02dc => return DecodeResult::new(self, Key::InputModeAnalog),
                0x0cb5 => return DecodeResult::new(self, Key::InputModeExtIn),
                0x02c3 => return DecodeResult::new(self, Key::InputPhono),
                0x02d2 => return DecodeResult::new(self, Key::InputTape),
                0x02c5 => return DecodeResult::new(self, Key::InputTuner),
                0x02c9 => return DecodeResult::new(self, Key::InputTv),
                0x02cc => return DecodeResult::new(self, Key::InputVAux),
                0x02cd => return DecodeResult::new(self, Key::InputVcr1),
                0x02ce => return DecodeResult::new(self, Key::InputVcr2),
                0x02ca => return DecodeResult::new(self, Key::InputVdp),
                0x0ca8 => return DecodeResult::new(self, Key::Mode5ch7ch),
                0x0495 => return DecodeResult::new(self, Key::ModeCinema),
                0x02e6 => return DecodeResult::new(self, Key::ModeDspSimu),
                0x0497 => return DecodeResult::new(self, Key::ModeGame),
                0x0496 => return DecodeResult::new(self, Key::ModeMusic),
                0x026a => return DecodeResult::new(self, Key::ModePureDirect),
                0x02e4 => return DecodeResult::new(self, Key::ModeStandard),
                0x0c9d => return DecodeResult::new(self, Key::ModeStereo),
                0x0c82 => return DecodeResult::new(self, Key::ModeSurroundBack),
                0x02df => return DecodeResult::new(self, Key::OnScreen),
                0x027a => return DecodeResult::new(self, Key::PowerOffMain),
                0x0279 => return DecodeResult::new(self, Key::PowerOnMain),
                0x0cd6 => return DecodeResult::new(self, Key::PresetNext),
                0x0cd5 => return DecodeResult::new(self, Key::PresetPrevious),
                0x049d => return DecodeResult::new(self, Key::RoomEq),
                0x02ed => return DecodeResult::new(self, Key::Speaker),
                0x0ca1 => return DecodeResult::new(self, Key::SurroundParameter),
                0x0ca0 => return DecodeResult::new(self, Key::SystemSetup),
                0x02ea => return DecodeResult::new(self, Key::TestTone),
                0x0cd7 => return DecodeResult::new(self, Key::TunerBand),
                0x0ccc => return DecodeResult::new(self, Key::TunerMemory),
                0x0cd8 => return DecodeResult::new(self, Key::TunerMode),
                0x0ccd => return DecodeResult::new(self, Key::TunerShift),
                0x0cb2 => return DecodeResult::new(self, Key::VideoOff),
                0x02d8 => return DecodeResult::new(self, Key::VideoSelect),
                0x02f1 => return DecodeResult::new(self, Key::VolumeUp),
                0x02f2 => return DecodeResult::new(self, Key::VolumeDown),
                0x0cd9 => return DecodeResult::new(self, Key::ChannelUp),
                0x0cda => return DecodeResult::new(self, Key::ChannelDown),
                0x0ca3 => return DecodeResult::new(self, Key::DirUp),
                0x0ca4 => return DecodeResult::new(self, Key::DirDown),
                0x0c7f => return DecodeResult::new(self, Key::DirLeft),
                0x02dd => return DecodeResult::new(self, Key::DirRight),
                0x02e0 => return DecodeResult::new(self, Key::Select),
                0x02f0 => return DecodeResult::new(self, Key::Mute),
                0x0cc1 => return DecodeResult::new(self, Key::Num1),
                0x0cc2 => return DecodeResult::new(self, Key::Num2),
                0x0cc3 => return DecodeResult::new(self, Key::Num3),
                0x0cc4 => return DecodeResult::new(self, Key::Num4),
                0x0cc5 => return DecodeResult::new(self, Key::Num5),
                0x0cc6 => return DecodeResult::new(self, Key::Num6),
                0x0cc7 => return DecodeResult::new(self, Key::Num7),
                0x0cc8 => return DecodeResult::new(self, Key::Num8),
                0x0cc9 => return DecodeResult::new(self, Key::Num9),
                0x0cca => return DecodeResult::new(self, Key::Num0),
                _ => return Option::None,
            }
        }
        return Option::None;
    }
}
