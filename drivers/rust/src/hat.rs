use crate::ButtonPress;
use crate::Config;
use crate::CurrentChannel;
use crate::RawHatError;
use crate::{RawHat, RawHatMessage};
use std::error::Error;
use std::fmt;
use std::sync::mpsc;
use std::time::Duration;
use std::time::SystemTime;
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
        callback: Box<dyn FnMut(HatMessage) + Send>,
    ) -> Hat {
        let mut local_callback = callback;

        let mut last_time = SystemTime::now();
        let mut last_remote_name: String = "".to_string();
        let mut last_button_name: String = "".to_string();

        let local_config = config.clone();
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
                        for remote_name in local_config.get_remote_names() {
                            let remote = local_config.get_remote(&remote_name).unwrap();
                            for button_name in remote.get_button_names() {
                                let button =
                                    local_config.get_button(&remote_name, &button_name).unwrap();
                                for button_signal in button.get_ir_signals() {
                                    if button_signal.get_protocol() == signal.protocol
                                        && button_signal.get_address() == signal.address
                                        && button_signal.get_command() == signal.command
                                    {
                                        let now = SystemTime::now();
                                        let time_since_last = now
                                            .duration_since(last_time)
                                            .unwrap_or(Duration::from_millis(1));
                                        last_time = now;
                                        last_remote_name = remote_name.to_string();
                                        last_button_name = button_name.to_string();
                                        if button.get_debounce().is_none()
                                            || time_since_last > button.get_debounce().unwrap()
                                        {
                                            local_callback(HatMessage::ButtonPress(ButtonPress {
                                                remote_name: remote_name.to_string(),
                                                button_name: button_name.to_string(),
                                            }));
                                        }
                                    }
                                }
                            }
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
        let _button = self
            .config
            .get_button(remote_name, button_name)
            .ok_or_else(|| HatError::InvalidButton {
                remote_name: remote_name.to_string(),
                button_name: button_name.to_string(),
            })?;
        // TODO
        // let signals: Vec<u32> = button.get_signals();

        // return self.send_signals(38000, &signals);
        return Result::Err(HatError::ErrResponse("TODO".to_string()));
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
