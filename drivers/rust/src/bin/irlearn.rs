use clap::App;
use clap::Arg;
use raspberry_pi_ir_hat::Signal;
use raspberry_pi_ir_hat::{Config, RawHat, RawHatMessage};
use std::sync::Arc;
use std::sync::Mutex;
use std::time::Duration;
use std::time::SystemTime;
use std::{thread, time};

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
    let tolerance = match args.value_of("tolerance").unwrap().parse::<f32>() {
        Ok(p) => p,
        _ => {
            println!("invalid tolerance: {}", args.value_of("tolerance").unwrap());
            std::process::exit(1);
        }
    };
    let minimum_signals = match args.value_of("minimumSignals").unwrap().parse::<u32>() {
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
        .parse::<u32>()
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

    let config = match Config::read(filename, false) {
        Ok(c) => c,
        Err(e) => {
            println!("{}", e);
            std::process::exit(1);
        }
    };

    let signal = Signal::new(
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
    println!("press ctrl+c to exit");
    loop {
        thread::sleep(Duration::from_millis(100));
        let mut t = timeout.lock().unwrap();
        if let Some(some_t) = *t {
            if SystemTime::now() > some_t {
                println!("timeout");
                *t = Option::None;
            }
        }
    }
}
