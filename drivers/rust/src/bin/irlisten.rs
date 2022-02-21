use clap::App;
use clap::Arg;
use log::info;
use raspberry_pi_ir_hat::{Config, Hat};
use simple_logger;
use std::{thread, time};

fn main() -> Result<(), String> {
    simple_logger::init_with_env().map_err(|err| format!("{}", err))?;
    info!("starting");

    let args = App::new("Raspberry Pi IrHat - irlisten")
        .version("1.0.0")
        .author("Joe Ferner <joe@fernsroth.com>")
        .about("Listen for remote button presses")
        .arg(
            Arg::with_name("file")
                .short("f")
                .long("file")
                .value_name("FILE")
                .help("File to create/add IR signals to")
                .required(true)
                .takes_value(true),
        )
        .arg(
            Arg::with_name("port")
                .short("p")
                .long("port")
                .default_value("/dev/serial0")
                .help("Path to serial port")
                .takes_value(true),
        )
        .get_matches();

    let filename = args.value_of("file").unwrap();
    let port = args.value_of("port").unwrap();

    let config =
        Config::read(filename, false).map_err(|err| format!("failed to read config {}", err))?;

    let mut hat = Hat::new(config, port);
    hat.open(Box::new(|msg| {
        println!("{:#?}", msg);
    }))
    .map_err(|err| format!("failed to open hat {}", err))?;
    println!("press ctrl+c to exit");
    loop {
        thread::sleep(time::Duration::from_secs(1));
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use raspberry_pi_ir_hat::hat::HatMessage::ButtonPress;
    use raspberry_pi_ir_hat::socat::socat;
    use std::sync::Arc;
    use std::sync::Mutex;
    use std::time::Duration;

    #[test]
    fn test_irlisten() {
        let mut socat_result = socat();
        let port = socat_result.get_port();
        let mut sp = socat_result.take_serial_port();

        let config = Config::from_str(
            r#"
remotes:
  denon:
    interval: 25
    repeat: 2
    buttons:
      power:
        debounce: 100
        ir_signals:
          - protocol: Denon
            address: 8
            command: 135
      volume_up:
        debounce: 10
        ir_signals:
          - protocol: Denon
            address: 8
            command: 79
      volume_down:
        debounce: 10
        ir_signals:
          - protocol: Denon
            address: 8
            command: 143
"#,
        )
        .unwrap();

        let messages = Arc::new(Mutex::new(Vec::new()));
        let hat_messages = messages.clone();
        let mut hat = Hat::new(config, &port);
        hat.open(Box::new(move |message| {
            hat_messages.lock().unwrap().push(message);
        }))
        .unwrap();

        thread::spawn(move || {
            thread::sleep(Duration::from_millis(10));
            sp.write("!s1,8,135,0,0,0,42\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(10));
            sp.write("!s1,8,135,0,0,0,42\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(10));
            sp.write("!s1,8,79,0,0,0,42\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(15));
            sp.write("!s1,8,143,0,0,0,42\n".as_bytes()).unwrap();
        });

        let mut message_index = 0;
        'outer: for _ in 0..100 {
            {
                thread::sleep(Duration::from_millis(10));
                let mut msgs = messages.lock().unwrap();
                while msgs.len() > 0 {
                    let message = msgs.remove(0);
                    if message_index == 0 {
                        if let ButtonPress(bp) = message {
                            assert_eq!("denon", bp.remote_name);
                            assert_eq!("power", bp.button_name);
                            message_index = message_index + 1;
                        } else {
                            panic!(
                                "unexpected message at offset {}: {:?}",
                                message_index, message
                            );
                        }
                    } else if message_index == 1 {
                        if let ButtonPress(bp) = message {
                            assert_eq!("denon", bp.remote_name);
                            assert_eq!("volume_up", bp.button_name);
                            message_index = message_index + 1;
                        } else {
                            panic!(
                                "unexpected message at offset {}: {:?}",
                                message_index, message
                            );
                        }
                    } else if message_index == 2 {
                        if let ButtonPress(bp) = message {
                            assert_eq!("denon", bp.remote_name);
                            assert_eq!("volume_down", bp.button_name);
                            message_index = message_index + 1;
                            break 'outer;
                        } else {
                            panic!(
                                "unexpected message at offset {}: {:?}",
                                message_index, message
                            );
                        }
                    } else {
                        panic!("unexpected message: {:?}", message);
                    }
                }
            }
        }
        assert_eq!(3, message_index);
    }
}
