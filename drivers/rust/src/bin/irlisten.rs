use clap::App;
use clap::Arg;
use raspberry_pi_ir_hat::{Config, Hat};
use std::{thread, time};

fn main() {
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
    let tolerance = match args.value_of("tolerance").unwrap().parse::<f32>() {
        Ok(p) => p,
        _ => {
            println!("invalid tolerance: {}", args.value_of("tolerance").unwrap());
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

    let mut hat = Hat::new(
        config,
        port,
        tolerance,
        Box::new(|message| {
            println!("{:#?}", message);
        }),
    );
    if let Err(e) = hat.open() {
        println!("{}", e);
        std::process::exit(1);
    }
    println!("press ctrl+c to exit");
    loop {
        thread::sleep(time::Duration::from_secs(1));
    }
}
