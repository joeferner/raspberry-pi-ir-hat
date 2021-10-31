use crate::aho_corasick::AhoCorasick;
use crate::ButtonPress;
use crate::Config;
use crate::CurrentChannel;
use crate::RawHatError;
use crate::{RawHat, RawHatMessage};
use std::error::Error;
use std::fmt;
use std::sync::mpsc;
use std::time::Duration;
use std::time::SystemTimeError;

#[derive(Debug)]
pub enum HatMessage {
    ButtonPress(ButtonPress),
    Error(String),
}

pub struct Hat {
    config: Config,
    response_queue: mpsc::Receiver<RawHatMessage>,
    raw_hat: RawHat,
    timeout: Duration,
}

pub struct Current {
    pub milliamps: u32,
}

#[derive(Debug)]
pub enum HatError {
    Timeout(mpsc::RecvTimeoutError),
    SystemTimeError(SystemTimeError),
    RawHatError(RawHatError),
    InvalidButton {
        remote_name: String,
        button_name: String,
    },
    ErrResponse(String),
}

impl fmt::Display for HatError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            HatError::Timeout(err) => write!(f, "timeout error: {}", err),
            HatError::SystemTimeError(err) => write!(f, "system time error: {}", err),
            HatError::RawHatError(err) => write!(f, "{}", err),
            HatError::ErrResponse(err) => write!(f, "invalid response: {}", err),
            HatError::InvalidButton {
                remote_name,
                button_name,
            } => write!(
                f,
                "invalid button (remote name: {}, button name: {})",
                remote_name, button_name
            ),
        }
    }
}

impl Error for HatError {}

impl Hat {
    pub fn new(
        config: Config,
        port_path: &str,
        tolerance: f32,
        callback: Box<dyn FnMut(HatMessage) + Send>,
    ) -> Hat {
        let mut local_callback = callback;
        let mut aho_corasick = AhoCorasick::new(&config, tolerance);
        let (sender, receiver) = mpsc::channel();
        return Hat {
            config,
            response_queue: receiver,
            timeout: Duration::from_secs(1),
            raw_hat: RawHat::new(
                port_path,
                Box::new(move |msg| match msg {
                    RawHatMessage::Ready => {}
                    RawHatMessage::UnknownLine(line) => {
                        local_callback(HatMessage::Error(format!("Unknown line: {}", line)));
                    }
                    RawHatMessage::Signal(signal) => {
                        let result = aho_corasick.append_find(signal);
                        if let Some(result) = result {
                            local_callback(HatMessage::ButtonPress(ButtonPress {
                                remote_name: result.remote_name.to_string(),
                                button_name: result.button_name.to_string(),
                            }));
                        }
                    }
                    RawHatMessage::OkResponse(message) => {
                        sender
                            .send(RawHatMessage::OkResponse(message))
                            .unwrap_or_else(|err| {
                                local_callback(HatMessage::Error(format!(
                                    "failed to queue response {}",
                                    err
                                )))
                            });
                    }
                    RawHatMessage::ErrResponse(err) => {
                        sender
                            .send(RawHatMessage::ErrResponse(err))
                            .unwrap_or_else(|err| {
                                local_callback(HatMessage::Error(format!(
                                    "failed to queue response {}",
                                    err
                                )))
                            });
                    }
                    RawHatMessage::Error(err) => {
                        local_callback(HatMessage::Error(err));
                    }
                }),
            ),
        };
    }

    pub fn get_config(&self) -> &Config {
        return &self.config;
    }

    pub fn open(&mut self) -> Result<(), HatError> {
        return self
            .raw_hat
            .open()
            .map_err(|err| HatError::RawHatError(err));
    }

    pub fn transmit(&mut self, remote_name: &str, button_name: &str) -> Result<(), HatError> {
        let button = self
            .config
            .get_button(remote_name, button_name)
            .ok_or_else(|| HatError::InvalidButton {
                remote_name: remote_name.to_string(),
                button_name: button_name.to_string(),
            })?;
        let signals: Vec<u32> = button.get_signals();

        return self.send_signals(38000, &signals);
    }

    fn send_signals(&mut self, frequency: u32, signals: &Vec<u32>) -> Result<(), HatError> {
        self.send_carrier_frequency(frequency)?;
        for s in signals {
            self.send_signal(*s)?;
        }
        return self.send_signal_complete();
    }

    fn send_carrier_frequency(&mut self, frequency: u32) -> Result<(), HatError> {
        self.raw_hat
            .send_carrier_frequency(frequency)
            .map_err(|err| HatError::RawHatError(err))?;
        return self.wait_for_response().map(|_| ());
    }

    fn send_signal(&mut self, signal: u32) -> Result<(), HatError> {
        self.raw_hat
            .send_signal(signal)
            .map_err(|err| HatError::RawHatError(err))?;
        return self.wait_for_response().map(|_| ());
    }

    fn send_signal_complete(&mut self) -> Result<(), HatError> {
        self.raw_hat
            .send_signal_complete()
            .map_err(|err| HatError::RawHatError(err))?;
        return self.wait_for_response().map(|_| ());
    }

    pub fn get_current(&mut self, channel: CurrentChannel) -> Result<Current, HatError> {
        self.raw_hat
            .send_get_current(channel)
            .map_err(|err| HatError::RawHatError(err))?;
        return self.wait_for_response().and_then(|value| match value {
            Option::None => Result::Err(HatError::ErrResponse(
                "invalid response for get current (no value)".to_string(),
            )),
            Option::Some(s) => Result::Ok(
                s.parse::<u32>()
                    .map(|v| Current { milliamps: v })
                    .map_err(|err| {
                        HatError::ErrResponse(format!("invalid response {}: {}", s, err))
                    })?,
            ),
        });
    }

    fn wait_for_response(&mut self) -> Result<Option<String>, HatError> {
        loop {
            match self
                .response_queue
                .recv_timeout(self.timeout)
                .map_err(|err| {
                    if let Result::Err(err) = self.reset() {
                        return err;
                    }
                    HatError::Timeout(err)
                })? {
                RawHatMessage::Ready => {}
                RawHatMessage::OkResponse(value) => return Result::Ok(value),
                RawHatMessage::ErrResponse(err) => return Result::Err(HatError::ErrResponse(err)),
                RawHatMessage::Error(err) => return Result::Err(HatError::ErrResponse(err)),
                RawHatMessage::Signal(_) => {
                    return Result::Err(HatError::ErrResponse(format!(
                        "unexpected signal response"
                    )))
                }
                RawHatMessage::UnknownLine(line) => {
                    return Result::Err(HatError::ErrResponse(format!("unknown line: {}", line)))
                }
            };
        }
    }

    pub fn reset(&mut self) -> Result<(), HatError> {
        return self
            .raw_hat
            .reset()
            .map_err(|err| HatError::RawHatError(err));
    }
}
