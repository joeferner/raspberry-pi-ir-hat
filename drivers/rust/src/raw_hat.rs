use rppal::gpio;
use rppal::gpio::OutputPin;
use serialport::SerialPort;
use std::error::Error;
use std::fmt;
use std::io::prelude::*;
use std::option::Option;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::thread;
use std::thread::JoinHandle;
use std::time::Duration;

#[cfg(target_arch = "arm")]
use rppal::gpio::{Gpio, OutputPin};

#[cfg(target_arch = "arm")]
const GPIO_RESET: u8 = 17;

#[derive(Debug)]
pub enum RawHatMessage {
    Ready,
    UnknownLine(String),
    Signal(u32),
    OkResponse(Option<String>),
    ErrResponse(String),
    Error(String),
}

pub struct RawHat {
    port_path: String,
    port: Option<Box<dyn SerialPort>>,
    read_thread: Option<JoinHandle<()>>,
    reset_pin: Option<OutputPin>,
    should_stop: Arc<AtomicBool>,
    callback: Option<Box<dyn FnMut(RawHatMessage) + Send>>,
}

#[derive(Debug)]
pub enum RawHatError {
    NotOpen,
    SerialPortError(serialport::Error),
    StdIoError(std::io::Error),
    WriteErrorLengthMismatch { expected: usize, actual: usize },
    GpioError(gpio::Error),
}

impl fmt::Display for RawHatError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            RawHatError::NotOpen => write!(f, "port not open"),
            RawHatError::SerialPortError(err) => write!(f, "serial port error: {}", err),
            RawHatError::StdIoError(err) => write!(f, "io error: {}", err),
            RawHatError::GpioError(err) => write!(f, "gpio error: {}", err),
            RawHatError::WriteErrorLengthMismatch { expected, actual } => write!(
                f,
                "write length mismatch, expected {}, actual {}",
                expected, actual
            ),
        }
    }
}

impl Error for RawHatError {}

impl RawHat {
    pub fn new(port_path: &str, callback: Box<dyn FnMut(RawHatMessage) + Send>) -> RawHat {
        return RawHat {
            port_path: port_path.to_string(),
            port: Option::None,
            read_thread: Option::None,
            reset_pin: Option::None,
            should_stop: Arc::new(AtomicBool::new(false)),
            callback: Option::Some(callback),
        };
    }

    pub fn open(&mut self) -> Result<(), RawHatError> {
        self.reset_pin = RawHat::get_reset_pin()?;

        let port = serialport::new(self.port_path.clone(), 57600)
            .timeout(Duration::from_secs(1))
            .open()
            .map_err(|err| RawHatError::SerialPortError(err))?;
        let mut read_port = port
            .try_clone()
            .map_err(|err| RawHatError::SerialPortError(err))?;
        self.port = Option::Some(port);
        self.should_stop.store(false, Ordering::Relaxed);
        let mut callback = self.callback.take().unwrap();
        let thread_should_stop = self.should_stop.clone();
        self.read_thread = Option::Some(thread::spawn(move || {
            let mut buffer = Vec::new();
            loop {
                if thread_should_stop.load(Ordering::Relaxed) {
                    break;
                }
                let mut temp = [0; 100];
                match read_port.read(&mut temp) {
                    Ok(d) => {
                        for i in 0..d {
                            let c = temp[i] as char;
                            if c == '\n' {
                                let line: String =
                                    buffer.iter().collect::<String>().trim().to_string();
                                if line.starts_with("!s") {
                                    parse_signal_line(line, &mut callback);
                                } else if line.starts_with("?READY") {
                                    callback(RawHatMessage::Ready);
                                } else if line.starts_with("+OK") {
                                    parse_ok_line(line, &mut callback);
                                } else if line.starts_with("-ERR") {
                                    parse_err_line(line, &mut callback);
                                } else {
                                    callback(RawHatMessage::UnknownLine(line));
                                }
                                buffer.clear();
                            }
                            buffer.push(c);
                        }
                    }
                    Err(err) => {
                        if err.kind() != std::io::ErrorKind::TimedOut {
                            callback(RawHatMessage::Error(format!("{}", err)));
                        }
                    }
                }
            }
        }));

        return Result::Ok(());
    }

    pub fn send_carrier_frequency(&mut self, frequency: u32) -> Result<(), RawHatError> {
        let s = format!("+f{}\n", frequency);
        return self.send(&s);
    }

    pub fn send_signal(&mut self, signal: u32) -> Result<(), RawHatError> {
        let s = format!("+s{}\n", signal);
        return self.send(&s);
    }

    pub fn send_signal_complete(&mut self) -> Result<(), RawHatError> {
        return self.send("+send\n");
    }

    pub fn send(&mut self, value: &str) -> Result<(), RawHatError> {
        let port = self.port.as_mut().ok_or(RawHatError::NotOpen)?;
        let value_bytes = value.as_bytes();
        let bytes_written = port
            .write(value_bytes)
            .map_err(|err| RawHatError::StdIoError(err))?;
        if bytes_written != value_bytes.len() {
            return Result::Err(RawHatError::WriteErrorLengthMismatch {
                expected: value_bytes.len(),
                actual: bytes_written,
            });
        }
        return Result::Ok(());
    }

    #[cfg(target_arch = "arm")]
    fn get_reset_pin() -> Result<Option<OutputPin>, RawHatError> {
        let mut reset_pin = Gpio::new()
            .map_err(|err| RawHatError::GpioError(err))?
            .get(GPIO_RESET)
            .map_err(|err| RawHatError::GpioError(err))?
            .into_output();
        reset_pin.set_low();
        thread::sleep(Duration::from_millis(500));
        reset_pin.set_high();
        return Result::Ok(Option::Some(reset_pin));
    }

    #[cfg(not(target_arch = "arm"))]
    fn get_reset_pin() -> Result<Option<OutputPin>, RawHatError> {
        return Result::Ok(Option::None);
    }

    #[cfg(target_arch = "arm")]
    pub fn reset(&mut self) -> Result<(), RawHatError> {
        let reset_pin = self.reset_pin.as_mut().ok_or(RawHatError::NotOpen)?;
        reset_pin.set_low();
        thread::sleep(Duration::from_millis(500));
        reset_pin.set_high();
        return Result::Ok(());
    }

    #[cfg(not(target_arch = "arm"))]
    pub fn reset(&mut self) -> Result<(), RawHatError> {
        return Result::Ok(());
    }
}

impl Drop for RawHat {
    fn drop(&mut self) {
        if self.read_thread.is_some() {
            self.should_stop.store(true, Ordering::Relaxed);
            self.read_thread.take().unwrap().join().unwrap();
        }
    }
}

fn parse_signal_line(line: String, callback: &mut Box<dyn FnMut(RawHatMessage) + Send>) {
    match line.strip_prefix("!s") {
        Option::Some(val_str) => match val_str.parse::<u32>() {
            Result::Ok(val) => callback(RawHatMessage::Signal(val)),
            Result::Err(err) => callback(RawHatMessage::Error(format!(
                "failed to parse line: {} => {}",
                line, err
            ))),
        },
        Option::None => {
            callback(RawHatMessage::UnknownLine(line));
        }
    }
}

fn parse_ok_line(line: String, callback: &mut Box<dyn FnMut(RawHatMessage) + Send>) {
    match line.strip_prefix("+OK") {
        Option::Some(val_str) => {
            let val_str = val_str.trim();
            if val_str.len() > 0 {
                callback(RawHatMessage::OkResponse(Option::Some(val_str.to_string())));
            } else {
                callback(RawHatMessage::OkResponse(Option::None));
            }
        }
        Option::None => {
            callback(RawHatMessage::UnknownLine(line));
        }
    }
}

fn parse_err_line(line: String, callback: &mut Box<dyn FnMut(RawHatMessage) + Send>) {
    match line.strip_prefix("-ERR") {
        Option::Some(val_str) => {
            let val_str = val_str.trim();
            callback(RawHatMessage::ErrResponse(val_str.to_string()));
        }
        Option::None => {
            callback(RawHatMessage::UnknownLine(line));
        }
    }
}
