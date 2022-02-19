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
use std::thread;
use std::thread::JoinHandle;
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
    read_thread: Option<JoinHandle<()>>,
}

struct HatRecvState {
    config: Arc<Mutex<Config>>,
    last_receive_time: SystemTime,
    last_remote_name: Option<String>,
    last_button_name: Option<String>,
    tx: mpsc::Sender<HatMessage>,
    response_queue_sender: mpsc::Sender<RawHatMessage>,
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
            read_thread: Option::None,
        };
    }

    fn handle_message(msg: RawHatMessage, hat_recv_state: &mut HatRecvState) {
        match msg {
            RawHatMessage::Ready => {}
            RawHatMessage::UnknownLine(line) => {
                hat_recv_state
                    .tx
                    .send(HatMessage::Error(format!("Unknown line: {}", line)))
                    .unwrap();
            }
            RawHatMessage::Signal(signal) => {
                Hat::handle_signal_message(signal, hat_recv_state);
            }
            RawHatMessage::OkResponse(message) => {
                hat_recv_state
                    .response_queue_sender
                    .send(RawHatMessage::OkResponse(message))
                    .unwrap_or_else(|err| {
                        hat_recv_state
                            .tx
                            .send(HatMessage::Error(format!(
                                "failed to queue response {}",
                                err
                            )))
                            .unwrap()
                    });
            }
            RawHatMessage::ErrResponse(err) => {
                hat_recv_state
                    .response_queue_sender
                    .send(RawHatMessage::ErrResponse(err))
                    .unwrap_or_else(|err| {
                        hat_recv_state
                            .tx
                            .send(HatMessage::Error(format!(
                                "failed to queue response {}",
                                err
                            )))
                            .unwrap()
                    });
            }
            RawHatMessage::Error(err) => {
                hat_recv_state.tx.send(HatMessage::Error(err)).unwrap();
            }
        }
    }

    fn handle_signal_message(signal: RawHatSignal, hat_recv_state: &mut HatRecvState) {
        let config = hat_recv_state.config.lock().unwrap();
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
                            .duration_since(hat_recv_state.last_receive_time)
                            .unwrap_or(Duration::from_millis(1));

                        if button.get_debounce().is_none()
                            || time_since_last > button.get_debounce().unwrap()
                        {
                            hat_recv_state
                                .tx
                                .send(HatMessage::ButtonPress(ButtonPress {
                                    remote_name: remote_name.to_string(),
                                    button_name: button_name.to_string(),
                                }))
                                .unwrap();
                        }

                        hat_recv_state.last_receive_time = now;
                        hat_recv_state.last_remote_name = Option::Some(remote_name.to_string());
                        hat_recv_state.last_button_name = Option::Some(button_name.to_string());
                    }
                }
            }
        }
    }

    pub fn get_config(&self) -> Arc<Mutex<Config>> {
        return self.config.clone();
    }

    pub fn open(&mut self) -> Result<mpsc::Receiver<HatMessage>, HatError> {
        let (tx, rx): (mpsc::Sender<HatMessage>, mpsc::Receiver<HatMessage>) = mpsc::channel();
        let (response_queue_sender, response_queue_receiver): (
            mpsc::Sender<RawHatMessage>,
            mpsc::Receiver<RawHatMessage>,
        ) = mpsc::channel();

        let raw_hat_rx = self
            .raw_hat
            .open()
            .map_err(|err| HatError::RawHatError(err))?;

        let config = self.config.clone();
        self.response_queue_receiver = Option::Some(response_queue_receiver);
        self.read_thread = Option::Some(thread::spawn(move || {
            let mut hat_recv_state = HatRecvState {
                config,
                last_receive_time: SystemTime::now(),
                last_remote_name: Option::None,
                last_button_name: Option::None,
                tx,
                response_queue_sender,
            };

            loop {
                let msg_result = raw_hat_rx.recv();
                match msg_result {
                    Result::Ok(msg) => Hat::handle_message(msg, &mut hat_recv_state),
                    Result::Err(err) => hat_recv_state
                        .tx
                        .send(HatMessage::Error(err.to_string()))
                        .unwrap(),
                }
            }
        }));
        return Result::Ok(rx);
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
