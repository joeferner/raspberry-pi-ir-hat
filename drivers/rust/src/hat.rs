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
    config: Arc<Mutex<Config>>,
    response_queue_receiver: Option<mpsc::Receiver<RawHatMessage>>,
    raw_hat: RawHat,
    timeout: Duration,
}

struct HatReceiveListener {
    config: Arc<Mutex<Config>>,
    last_receive_time: SystemTime,
    last_remote_name: Option<String>,
    last_button_name: Option<String>,
    response_queue_sender: mpsc::Sender<RawHatMessage>,
    callback: Arc<Mutex<Box<dyn FnMut(HatMessage) + Send>>>,
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
    pub fn new(config: Config, port_path: &str) -> Hat {
        return Hat {
            config: Arc::new(Mutex::new(config)),
            response_queue_receiver: Option::None,
            timeout: Duration::from_secs(1),
            raw_hat: RawHat::new(port_path),
        };
    }

    pub fn get_config(&self) -> Arc<Mutex<Config>> {
        return self.config.clone();
    }

    pub fn open(&mut self, callback: Box<dyn FnMut(HatMessage) + Send>) -> Result<(), HatError> {
        let (response_queue_sender, response_queue_receiver): (
            mpsc::Sender<RawHatMessage>,
            mpsc::Receiver<RawHatMessage>,
        ) = mpsc::channel();
        let config = self.config.clone();
        self.response_queue_receiver = Option::Some(response_queue_receiver);

        let mut hat_receive_listener = HatReceiveListener {
            config,
            last_receive_time: SystemTime::now() - Duration::from_secs(600),
            last_remote_name: Option::None,
            last_button_name: Option::None,
            response_queue_sender,
            callback: Arc::new(Mutex::new(callback)),
        };
        self.raw_hat
            .open(Box::new(move |msg| {
                hat_receive_listener.handle_message(msg);
            }))
            .map_err(|err| HatError::RawHatError(err))?;

        return Result::Ok(());
    }

    pub fn transmit(&mut self, remote_name: &str, button_name: &str) -> Result<(), HatError> {
        let config = self.config.lock().unwrap();

        let _button =
            config
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
        let timeout = self.timeout;
        loop {
            if let Option::Some(response_queue_receiver) = &self.response_queue_receiver {
                match response_queue_receiver
                    .recv_timeout(timeout)
                    .map_err(|err| {
                        if let Result::Err(err) = self.reset() {
                            return err;
                        }
                        HatError::Timeout(err)
                    })? {
                    RawHatMessage::Ready => {}
                    RawHatMessage::OkResponse(value) => return Result::Ok(value),
                    RawHatMessage::ErrResponse(err) => {
                        return Result::Err(HatError::ErrResponse(err))
                    }
                    RawHatMessage::Error(err) => return Result::Err(HatError::ErrResponse(err)),
                    RawHatMessage::Signal(_) => {
                        return Result::Err(HatError::ErrResponse(format!(
                            "unexpected signal response"
                        )))
                    }
                    RawHatMessage::UnknownLine(line) => {
                        return Result::Err(HatError::ErrResponse(format!(
                            "unknown line: {}",
                            line
                        )))
                    }
                };
            } else {
                return Result::Err(HatError::InvalidState);
            }
        }
    }

    pub fn reset(&mut self) -> Result<(), HatError> {
        return self
            .raw_hat
            .reset()
            .map_err(|err| HatError::RawHatError(err));
    }
}

impl HatReceiveListener {
    fn handle_message(&mut self, msg: RawHatMessage) {
        match msg {
            RawHatMessage::Ready => {}
            RawHatMessage::UnknownLine(line) => {
                self.on_hat_message(HatMessage::Error(format!("Unknown line: {}", line)));
            }
            RawHatMessage::Signal(signal) => {
                self.handle_signal_message(signal);
            }
            RawHatMessage::OkResponse(message) => {
                self.response_queue_sender
                    .send(RawHatMessage::OkResponse(message))
                    .unwrap_or_else(|err| {
                        self.on_hat_message(HatMessage::Error(format!(
                            "failed to queue response {}",
                            err
                        )));
                    });
            }
            RawHatMessage::ErrResponse(err) => {
                self.response_queue_sender
                    .send(RawHatMessage::ErrResponse(err))
                    .unwrap_or_else(|err| {
                        self.on_hat_message(HatMessage::Error(format!(
                            "failed to queue response {}",
                            err
                        )))
                    });
            }
            RawHatMessage::Error(err) => {
                self.on_hat_message(HatMessage::Error(err));
            }
        }
    }

    fn handle_signal_message(&mut self, signal: RawHatSignal) {
        let config = self.config.lock().unwrap();
        for remote_name in config.get_remote_names() {
            let remote = config.get_remote(&remote_name).unwrap();
            for button_name in remote.get_button_names() {
                let button = config.get_button(&remote_name, &button_name).unwrap();
                for button_signal in button.get_ir_signals() {
                    if button_signal.get_protocol() == signal.protocol
                        && button_signal.get_address() == signal.address
                        && button_signal.get_command() == signal.command
                    {
                        let now = SystemTime::now();
                        let time_since_last = now
                            .duration_since(self.last_receive_time)
                            .unwrap_or(Duration::from_millis(1));

                        if button.get_debounce().is_none()
                            || time_since_last > button.get_debounce().unwrap()
                        {
                            self.on_hat_message(HatMessage::ButtonPress(ButtonPress {
                                remote_name: remote_name.to_string(),
                                button_name: button_name.to_string(),
                            }));
                        }

                        self.last_receive_time = now;
                        self.last_remote_name = Option::Some(remote_name.to_string());
                        self.last_button_name = Option::Some(button_name.to_string());
                    }
                }
            }
        }
    }

    fn on_hat_message(&self, msg: HatMessage) -> () {
        let mut callback = self.callback.lock().unwrap();
        callback(msg);
    }
}
