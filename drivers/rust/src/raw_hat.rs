use serialport::SerialPort;
use std::io::prelude::*;
use std::option::Option;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::thread;
use std::thread::JoinHandle;
use std::time::Duration;

#[derive(Debug)]
pub enum RawHatMessage {
    Line(String),
    Error(std::io::Error),
}

pub struct RawHat {
    port_path: String,
    port: Option<Box<dyn SerialPort>>,
    read_thread: Option<JoinHandle<()>>,
    should_stop: Arc<AtomicBool>,
    callback: Option<Box<dyn FnMut(RawHatMessage) + Send>>,
}

impl RawHat {
    pub fn new(port_path: &str, callback: Box<dyn FnMut(RawHatMessage) + Send>) -> RawHat {
        return RawHat {
            port_path: port_path.to_string(),
            port: Option::None,
            read_thread: Option::None,
            should_stop: Arc::new(AtomicBool::new(false)),
            callback: Option::Some(callback),
        };
    }

    pub fn open(&mut self) -> Result<(), String> {
        let port = match serialport::new(self.port_path.clone(), 57600)
            .timeout(Duration::from_secs(1))
            .open()
        {
            Ok(p) => p,
            Err(e) => {
                return Result::Err(format!("failed to open port: {}", e));
            }
        };
        let mut read_port = match port.try_clone() {
            Ok(p) => p,
            Err(e) => {
                return Result::Err(format!("failed to clone open port: {}", e));
            }
        };
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
                                let line: String = buffer.iter().collect();
                                callback(RawHatMessage::Line(line));
                                buffer.clear();
                            }
                            buffer.push(c);
                        }
                    }
                    Err(err) => {
                        if err.kind() != std::io::ErrorKind::TimedOut {
                            callback(RawHatMessage::Error(err));
                        }
                    }
                }
            }
        }));

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
