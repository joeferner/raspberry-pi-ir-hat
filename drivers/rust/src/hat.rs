use crate::raw_hat::RawHatSignal;
use crate::ButtonPress;
use crate::Config;
use crate::CurrentChannel;
use crate::RawHatError;
use crate::{RawHat, RawHatMessage};
use std::error::Error;
use std::fmt;
use std::sync::mpsc;
use std::sync::Arc;
use std::sync::Mutex;
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
    response_queue_receiver: mpsc::Receiver<RawHatMessage>,
    raw_hat: Option<RawHat>,
    timeout: Duration,

    last_recieve_time: SystemTime,
    last_remote_name: Option<String>,
    last_button_name: Option<String>,
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
    InvalidState,
}

impl fmt::Display for HatError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            HatError::Timeout(err) => write!(f, "timeout error: {}", err),
            HatError::SystemTimeError(err) => write!(f, "system time error: {}", err),
            HatError::RawHatError(err) => write!(f, "{}", err),
            HatError::ErrResponse(err) => write!(f, "invalid response: {}", err),
            HatError::InvalidState => write!(f, "invalid state"),
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
    ) -> Arc<Mutex<Hat>> {
        let (sender, receiver) = mpsc::channel();

        let hat = Arc::new(Mutex::new(Hat {
            config,
            response_queue_receiver: receiver,
            timeout: Duration::from_secs(1),
            raw_hat: Option::None,

            last_recieve_time: SystemTime::now(),
            last_remote_name: Option::None,
            last_button_name: Option::None,
        }));

        let local_hat = hat.clone();
        let mut local_callback = callback;
        let local_sender = sender;
        let handle_message_callback = Box::new(move |msg| {
            Hat::handle_message(&local_hat, &mut local_callback, &local_sender, msg);
        });

        {
            let mut my_hat = hat.lock().unwrap();
            my_hat.raw_hat = Option::Some(RawHat::new(port_path, handle_message_callback));
        }

        return hat;
    }

    fn handle_message(
        hat: &Arc<Mutex<Hat>>,
        callback: &mut Box<dyn FnMut(HatMessage) + Send>,
        response_queue_sender: &mpsc::Sender<RawHatMessage>,
        msg: RawHatMessage,
    ) {
        match msg {
            RawHatMessage::Ready => {}
            RawHatMessage::UnknownLine(line) => {
                callback(HatMessage::Error(format!("Unknown line: {}", line)));
            }
            RawHatMessage::Signal(signal) => {
                let mut my_hat = hat.lock().unwrap();
                my_hat.handle_signal_message(signal, callback);
            }
            RawHatMessage::OkResponse(message) => {
                response_queue_sender
                    .send(RawHatMessage::OkResponse(message))
                    .unwrap_or_else(|err| {
                        callback(HatMessage::Error(format!(
                            "failed to queue response {}",
                            err
                        )))
                    });
            }
            RawHatMessage::ErrResponse(err) => {
                response_queue_sender
                    .send(RawHatMessage::ErrResponse(err))
                    .unwrap_or_else(|err| {
                        callback(HatMessage::Error(format!(
                            "failed to queue response {}",
                            err
                        )))
                    });
            }
            RawHatMessage::Error(err) => {
                callback(HatMessage::Error(err));
            }
        }
    }

    fn handle_signal_message(
        &mut self,
        signal: RawHatSignal,
        callback: &mut Box<dyn FnMut(HatMessage) + Send>,
    ) {
        for remote_name in self.config.get_remote_names() {
            let remote = self.config.get_remote(&remote_name).unwrap();
            for button_name in remote.get_button_names() {
                let button = self.config.get_button(&remote_name, &button_name).unwrap();
                for button_signal in button.get_ir_signals() {
                    if button_signal.get_protocol() == signal.protocol
                        && button_signal.get_address() == signal.address
                        && button_signal.get_command() == signal.command
                    {
                        let now = SystemTime::now();
                        let time_since_last = now
                            .duration_since(self.last_recieve_time)
                            .unwrap_or(Duration::from_millis(1));

                        if button.get_debounce().is_none()
                            || time_since_last > button.get_debounce().unwrap()
                        {
                            callback(HatMessage::ButtonPress(ButtonPress {
                                remote_name: remote_name.to_string(),
                                button_name: button_name.to_string(),
                            }));
                        }

                        self.last_recieve_time = now;
                        self.last_remote_name = Option::Some(remote_name.to_string());
                        self.last_button_name = Option::Some(button_name.to_string());
                    }
                }
            }
        }
    }

    pub fn get_config(&self) -> &Config {
        return &self.config;
    }

    pub fn open(&mut self) -> Result<(), HatError> {
        if let Option::Some(raw_hat) = self.raw_hat.as_mut() {
            return raw_hat.open().map_err(|err| HatError::RawHatError(err));
        }
        return Result::Err(HatError::InvalidState);
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
        if let Option::Some(raw_hat) = self.raw_hat.as_mut() {
            raw_hat
                .send_get_current(channel)
                .map_err(|err| HatError::RawHatError(err))?;
            return self.wait_for_response().and_then(|value| match value {
                Option::None => Result::Err(HatError::ErrResponse(
                    "invalid response for get current (no value)".to_string(),
                )),
                Option::Some(s) => {
                    Result::Ok(s.parse::<u32>().map(|v| Current { milliamps: v }).map_err(
                        |err| HatError::ErrResponse(format!("invalid response {}: {}", s, err)),
                    )?)
                }
            });
        }
        return Result::Err(HatError::InvalidState);
    }

    fn wait_for_response(&mut self) -> Result<Option<String>, HatError> {
        loop {
            match self
                .response_queue_receiver
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
        if let Option::Some(raw_hat) = self.raw_hat.as_mut() {
            return raw_hat.reset().map_err(|err| HatError::RawHatError(err));
        }
        return Result::Err(HatError::InvalidState);
    }
}
