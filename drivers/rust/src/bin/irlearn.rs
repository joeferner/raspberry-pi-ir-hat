use clap::App;
use clap::Arg;
use raspberry_pi_ir_hat::Signal;
use raspberry_pi_ir_hat::{Config, RawHat, RawHatMessage};
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time::Duration;
use std::time::SystemTime;

fn main() {
    let args = App::new("Raspberry Pi IrHat - irlearn")
        .version("1.0.0")
        .author("Joe Ferner <joe@fernsroth.com>")
        .about("Learn a new remote button")
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
        .arg(
            Arg::with_name("remote")
                .short("r")
                .long("remote")
                .required(true)
                .help("Name of the remote to learn")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("button")
                .short("b")
                .long("button")
                .required(true)
                .help("Name of the button to learn")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("debounce")
                .long("debounce")
                .help("Number of milliseconds between consecutive presses of this button")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("minimumSignals")
                .long("minimumSignals")
                .help("Minimum number of signals required")
                .default_value("5")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("numberOfMatchingSignalsByLength")
                .long("numberOfMatchingSignalsByLength")
                .help("Number of matching signals by length")
                .default_value("5")
                .takes_value(true),
        )
        .arg(
            Arg::with_name("timeout")
                .long("timeout")
                .help("Maximum time in milliseconds for an IR signal")
                .default_value("500")
                .takes_value(true),
        )
        .get_matches();

    let filename = args.value_of("file").unwrap();
    let port = args.value_of("port").unwrap();
    let remote = args.value_of("remote").unwrap();
    let button = args.value_of("button").unwrap();
    let tolerance = match args.value_of("tolerance").unwrap().parse::<f32>() {
        Ok(p) => p,
        _ => {
            println!("invalid tolerance: {}", args.value_of("tolerance").unwrap());
            std::process::exit(1);
        }
    };
    let minimum_signals = match args.value_of("minimumSignals").unwrap().parse::<usize>() {
        Ok(p) => p,
        _ => {
            println!(
                "invalid minimumSignals: {}",
                args.value_of("minimumSignals").unwrap()
            );
            std::process::exit(1);
        }
    };
    let number_of_matching_signals_by_length = match args
        .value_of("numberOfMatchingSignalsByLength")
        .unwrap()
        .parse::<usize>()
    {
        Ok(p) => p,
        _ => {
            println!(
                "invalid numberOfMatchingSignalsByLength: {}",
                args.value_of("numberOfMatchingSignalsByLength").unwrap()
            );
            std::process::exit(1);
        }
    };
    let timeout_duration = match args.value_of("timeout").unwrap().parse::<u64>() {
        Ok(p) => Duration::from_millis(p),
        _ => {
            println!("invalid timeout: {}", args.value_of("timeout").unwrap());
            std::process::exit(1);
        }
    };
    let debounce = match args.value_of("debounce") {
        Option::None => Option::None,
        Option::Some(s) => match s.parse::<u32>() {
            Ok(v) => Option::Some(v),
            _ => {
                println!("invalid debounce: {}", args.value_of("debounce").unwrap());
                std::process::exit(1);
            }
        },
    };

    let mut config = match Config::read(filename, false) {
        Ok(c) => c,
        Err(e) => {
            println!("{}", e);
            std::process::exit(1);
        }
    };

    let results = learn(
        port,
        tolerance,
        minimum_signals,
        number_of_matching_signals_by_length,
        timeout_duration,
        remote,
        button,
    );

    config.set_button(remote, button, &results, debounce);
    match config.write(filename) {
        Result::Ok(_) => {
            println!("button saved");
            std::process::exit(0);
        }
        Result::Err(err) => {
            println!("{}", err);
            std::process::exit(1);
        }
    };
}

fn learn(
    port: &str,
    tolerance: f32,
    minimum_signals: usize,
    number_of_matching_signals_by_length: usize,
    timeout_duration: Duration,
    remote: &str,
    button: &str,
) -> String {
    let mut signal = Signal::new(
        tolerance,
        minimum_signals,
        number_of_matching_signals_by_length,
    );
    let current_signal = Arc::new(Mutex::new(Vec::new()));
    let timeout = Arc::new(Mutex::new(Option::None));

    let hat_current_signal = current_signal.clone();
    let hat_timeout = timeout.clone();
    let mut hat = RawHat::new(
        port,
        Box::new(move |message| match message {
            RawHatMessage::UnknownLine(line) => {
                println!("unknown line: {}", line);
            }
            RawHatMessage::Signal(s) => {
                hat_current_signal.lock().unwrap().push(s);
                let mut timeout = hat_timeout.lock().unwrap();
                *timeout = Option::Some(SystemTime::now() + timeout_duration);
            }
            RawHatMessage::OkResponse(_) => {}
            RawHatMessage::ErrResponse(err) => {
                println!("error response: {}", err);
            }
            RawHatMessage::Error(err) => {
                println!("error: {}", err);
            }
        }),
    );
    if let Err(e) = hat.open() {
        println!("{}", e);
        std::process::exit(1);
    }
    println!("press ctrl+c to cancel and exit");
    loop {
        thread::sleep(Duration::from_millis(100));
        let mut t = timeout.lock().unwrap();
        if let Some(some_t) = *t {
            if SystemTime::now() > some_t {
                let mut s = current_signal.lock().unwrap();
                match signal.push(&*s) {
                    Result::Ok(results) => {
                        return results;
                    }
                    Result::Err(err) => {
                        println!("{}", err);
                        println!(
                            "Press the \"{}\" on the \"{}\" remote again",
                            button, remote
                        );
                    }
                }
                *s = Vec::new();
                *t = Option::None;
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use raspberry_pi_ir_hat::socat::socat;
    use std::sync::atomic::{AtomicBool, Ordering};

    #[test]
    fn test_irlearn() {
        let mut socat_result = socat();
        let port = socat_result.get_port();
        let mut sp = socat_result.get_serial_port();
        let complete = Arc::new(Mutex::new(AtomicBool::new(false)));

        let thread_complete = complete.clone();
        thread::spawn(move || {
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            // TODO shouldn't need this extra signal here
            sp.write("!s100\n!s200\n!s300\n".as_bytes()).unwrap();
            thread::sleep(Duration::from_millis(100));
            assert_eq!(
                true,
                thread_complete.lock().unwrap().load(Ordering::Relaxed)
            );
        });

        let results = learn(
            &port,
            0.15,
            3,
            3,
            Duration::from_millis(50),
            "remote1",
            "button1",
        );
        assert_eq!("100,200,300", results);
        complete.lock().unwrap().store(true, Ordering::Relaxed);
    }
}
