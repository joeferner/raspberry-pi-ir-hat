use clap::App;
use clap::Arg;
use raspberry_pi_ir_hat::{Config, Hat};
use std::{thread, time};

fn main() -> Result<(), String> {
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
        .arg(
            Arg::with_name("tolerance")
                .short("t")
                .long("tolerance")
                .default_value("0.15")
                .help("Signal matching tolerance")
                .takes_value(true),
        )
        .get_matches();

    let filename = args.value_of("file").unwrap();
    let port = args.value_of("port").unwrap();
    let tolerance = args
        .value_of("tolerance")
        .unwrap()
        .parse::<f32>()
        .map_err(|err| {
            format!(
                "invalid tolerance: {} ({})",
                args.value_of("tolerance").unwrap(),
                err
            )
        })?;

    let config =
        Config::read(filename, false).map_err(|err| format!("failed to read config {}", err))?;

    let mut hat = Hat::new(
        config,
        port,
        tolerance,
        Box::new(|message| {
            println!("{:#?}", message);
        }),
    );
    hat.open()
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
    use raspberry_pi_ir_hat::ConfigButton;
    use raspberry_pi_ir_hat::ConfigRemote;
    use std::collections::HashMap;
    use std::sync::Arc;
    use std::sync::Mutex;
    use std::time::Duration;

    #[test]
    fn test_irlisten() {
        let mut socat_result = socat();
        let port = socat_result.get_port();
        let mut sp = socat_result.take_serial_port();

        let mut remote1_buttons: HashMap<String, ConfigButton> = HashMap::new();
        remote1_buttons.insert(
            "button1".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "100,200,300".to_string(),
            },
        );
        remote1_buttons.insert(
            "button2".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "100,300,300".to_string(),
            },
        );
        remote1_buttons.insert(
            "button3".to_string(),
            ConfigButton {
                debounce: Option::None,
                signal: "200,500,600,700".to_string(),
            },
        );

        let remote1 = ConfigRemote {
            buttons: remote1_buttons,
        };

        let mut remotes: HashMap<String, ConfigRemote> = HashMap::new();
        remotes.insert("remote1".to_string(), remote1);

        let config = Config { remotes };

        thread::spawn(move || {
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            sp.write("!s100\n!s300\n!s300\n".as_bytes()).unwrap();
        });

        let messages = Arc::new(Mutex::new(Vec::new()));
        let hat_messages = messages.clone();
        let mut hat = Hat::new(
            config,
            &port,
            0.15,
            Box::new(move |message| {
                hat_messages.lock().unwrap().push(message);
            }),
        );
        hat.open().unwrap();

        let mut message_index = 0;
        'outer: for _ in 0..100 {
            thread::sleep(Duration::from_millis(100));
            {
                let mut msgs = messages.lock().unwrap();
                while msgs.len() > 0 {
                    let message = msgs.remove(0);
                    if message_index == 0 || message_index == 1 {
                        if let ButtonPress(bp) = message {
                            assert_eq!("remote1", bp.remote_name);
                            assert_eq!("button1", bp.button_name);
                            message_index = message_index + 1;
                        } else {
                            panic!(
                                "unexpected message at offset {}: {:?}",
                                message_index, message
                            );
                        }
                    } else if message_index == 2 {
                        if let ButtonPress(bp) = message {
                            assert_eq!("remote1", bp.remote_name);
                            assert_eq!("button2", bp.button_name);
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
