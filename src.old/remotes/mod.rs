use std::thread::sleep;
use std::time::Duration;

use self::denon::DenonRemote;
use self::pioneer::PioneerRemote;
use self::rca::RcaRemote;
use crate::lirc::{lirc_writer::LircWriter, LircEvent, LircProtocol};
use crate::my_error::{MyError, Result};
use log;

mod denon;
mod pioneer;
mod rca;

#[derive(Debug, Clone)]
pub enum Key {
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    VolumeUp,
    VolumeDown,
    ChannelUp,
    ChannelDown,
    PowerOn,
    PowerOff,
    PowerToggle,
    DirRight,
    DirLeft,
    DirUp,
    DirDown,
    Select,
    Record,
    InputAnt,
    Mute,
    Display,
    Dot,
    ChannelEnter,
    ChannelReturn,
    InputPc,
    Input1,
    Input2,
    Input3,
    Input4,
    Input5,
    Input6,
    Home,
    Back,
    Guide,
    Info,
    Menu,
    DayPlus,
    DayMinus,
    FavoriteB,
    FavoriteC,
    FavoriteD,
    FavoriteA,
    PageUp,
    PageDown,
    Size,
    AvSelection,
    Sleep,
    Dimmer,
    InputCd,
    InputDbs,
    InputDvd,
    InputMode,
    InputModeAnalog,
    InputModeExtIn,
    InputPhono,
    InputTape,
    InputTuner,
    InputTv,
    InputVAux,
    InputVcr1,
    InputVcr2,
    InputVdp,
    Mode5ch7ch,
    ModeCinema,
    ModeDspSimu,
    ModeGame,
    ModeMusic,
    ModePureDirect,
    ModeStandard,
    ModeStereo,
    ModeSurroundBack,
    OnScreen,
    PowerOffMain,
    PowerOnMain,
    PresetNext,
    PresetPrevious,
    RoomEq,
    Speaker,
    SurroundParameter,
    SystemSetup,
    TestTone,
    TunerBand,
    TunerMemory,
    TunerMode,
    TunerShift,
    VideoOff,
    VideoSelect,
}

#[derive(Debug, Clone)]
pub struct DecodeResult {
    pub time: u64,
    pub key: Key,
    pub repeat: u8,
    pub source: String,
}

impl DecodeResult {
    pub fn new(remote: &dyn Remote, key: Key) -> Option<Self> {
        return Option::Some(DecodeResult {
            time: 0,
            key,
            repeat: 0,
            source: remote.get_display_name().to_string(),
        });
    }
}

pub trait Remote: Send {
    fn get_protocol(&self) -> LircProtocol;
    fn get_repeat_count(&self) -> u32;
    fn get_tx_scan_code_gap(&self) -> Duration;
    fn get_tx_repeat_gap(&self) -> Duration;
    fn get_rx_repeat_gap_max(&self) -> Duration;
    fn get_display_name(&self) -> &str;

    fn decode(&self, events: &Vec<LircEvent>) -> Option<DecodeResult>;
    fn send(&self, writer: &mut LircWriter, key: Key) -> Result<()>;
}

pub struct Remotes {
    events: Vec<LircEvent>,
    last_decode_result: Option<DecodeResult>,
    remotes: Vec<Box<dyn Remote>>,
}

impl Remotes {
    pub fn new() -> Self {
        return Remotes {
            events: vec![],
            last_decode_result: Option::None,
            remotes: vec![
                Box::new(PioneerRemote::new()),
                Box::new(DenonRemote::new()),
                Box::new(RcaRemote::new()),
            ],
        };
    }

    pub fn send(&self, writer: &mut LircWriter, remote_name: &str, key: Key) -> Result<()> {
        for remote in &self.remotes {
            if remote.get_display_name() == remote_name {
                return remote.send(writer, key);
            }
        }
        return Result::Err(MyError::new(format!(
            "could not find remote {}",
            remote_name
        )));
    }

    pub fn decode(&mut self, event: LircEvent) -> Option<DecodeResult> {
        if let Option::Some(last_event) = self.events.last() {
            let delta_t = event.timestamp - last_event.timestamp;
            if delta_t > 500 * 1_000_000 {
                log::debug!("clearing {:?}", self.events);
                self.events.clear();
            }
        }

        self.events.push(event);

        for remote in &self.remotes {
            if self
                .events
                .iter()
                .all(|e| e.rc_protocol == remote.get_protocol() as u16)
            {
                if let Option::Some(mut result) = remote.decode(&self.events) {
                    result.time = (self.events.last().unwrap().timestamp / 1_000_000) as u64;
                    if let Option::Some(last_result) = &self.last_decode_result {
                        let delta_t = Duration::from_millis(result.time - last_result.time);
                        if delta_t < remote.get_rx_repeat_gap_max() {
                            result.repeat = last_result.repeat + 1;
                        }
                    }
                    self.events.clear();
                    self.last_decode_result = Option::Some(result.clone());
                    return Option::Some(result);
                }
            }
        }

        return Option::None;
    }
}

pub fn send_scan_codes(
    writer: &mut LircWriter,
    remote: &dyn Remote,
    scan_codes: Vec<u64>,
) -> Result<()> {
    for _repeat_index in 0..remote.get_repeat_count() {
        for (scan_code_index, scan_code) in scan_codes.iter().enumerate() {
            if scan_code_index > 0 {
                sleep(remote.get_tx_scan_code_gap());
            }
            writer.write(remote.get_protocol() as u16, *scan_code)?;
        }
        sleep(remote.get_tx_repeat_gap());
    }
    return Result::Ok(());
}
