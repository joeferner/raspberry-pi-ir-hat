use serialport::SerialPort;
use std::io::prelude::*;
use std::io::BufReader;
use std::process::Command;
use std::process::Stdio;
use std::time::Duration;

pub struct SocatResult {
    port: Option<String>,
    serial_port: Option<Box<dyn SerialPort>>,
    stop: Box<dyn FnMut()>,
}

impl SocatResult {
    pub fn get_port(&mut self) -> String {
        return self.port.take().unwrap();
    }

    pub fn get_serial_port(&mut self) -> Box<dyn SerialPort> {
        return self.serial_port.take().unwrap();
    }
}

impl Drop for SocatResult {
    fn drop(&mut self) {
        (self.stop)();
    }
}

pub fn socat() -> SocatResult {
    let mut port1: Option<String> = Option::None;
    let port2: Option<String>;
    let child = Command::new("socat")
        .args(["-d", "-d", "pty,raw,echo=0", "pty,raw,echo=0"])
        .stderr(Stdio::piped())
        .spawn()
        .unwrap();
    let child_pid = child.id();
    let mut stdout = BufReader::new(child.stderr.unwrap());
    loop {
        let mut line = String::new();
        stdout.read_line(&mut line).unwrap();

        if let Option::Some(pos) = line.find("PTY is") {
            let port = line.split_at(pos + "PTY is".len()).1.trim();
            if port1.is_none() {
                port1 = Option::Some(port.to_string());
            } else {
                port2 = Option::Some(port.to_string());
                break;
            }
        }
        line.clear();
    }
    let sp = serialport::new(port2.unwrap(), 57600)
        .timeout(Duration::from_secs(1))
        .open()
        .unwrap();
    return SocatResult {
        port: Option::Some(port1.unwrap()),
        serial_port: Option::Some(sp),
        stop: Box::new(move || {
            Command::new("kill")
                .args([child_pid.to_string()])
                .spawn()
                .unwrap();
        }),
    };
}
