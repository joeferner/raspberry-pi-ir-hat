use clap::App;
use clap::Arg;
use raspberry_pi_ir_hat::{Config, Hat};

fn main() -> Result<(), String> {
    let args = App::new("Raspberry Pi IrHat - irplay")
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
            Arg::with_name("remote")
                .short("r")
                .long("remote")
                .help("Name of the remote to play")
                .required(true)
                .takes_value(true),
        )
        .arg(
            Arg::with_name("button")
                .short("b")
                .long("button")
                .help("Name of the button to play")
                .required(true)
                .takes_value(true),
        )
        .get_matches();

    let filename = args.value_of("file").unwrap();
    let port = args.value_of("port").unwrap();
    let remote = args.value_of("remote").unwrap();
    let button = args.value_of("button").unwrap();

    let config =
        Config::read(filename, false).map_err(|err| format!("failed to read config {}", err))?;

    let mut hat = Hat::new(
        config,
        port,
        0.15,
        Box::new(|message| {
            println!("{:#?}", message);
        }),
    );
    hat.open()
        .map_err(|err| format!("failed to open hat {}", err))?;
    hat.transmit(remote, button)
        .map_err(|err| format!("failed to transmit {}", err))?;
    return Result::Ok(());
}

#[cfg(test)]
mod tests {
    use super::*;
    use raspberry_pi_ir_hat::socat::socat;
    use std::io::Read;
    use std::str;
    use std::thread;

    #[test]
    fn test_irplay() {
        let mut socat_result = socat();
        let port = socat_result.get_port();
        let mut sp = socat_result.take_serial_port();

        let config = Config::from_str(
            r#"
remotes:
  remote1:
    buttons:
      button1:
        signal: "100,200,300"
"#,
        )
        .unwrap();

        thread::spawn(move || {
            let read_line = |sp: &mut dyn Read| {
                let mut buf = [0; 100];
                let bytes_read = sp.read(&mut buf).unwrap();
                return str::from_utf8(&buf[0..bytes_read]).unwrap().to_string();
            };

            let line = read_line(&mut sp);
            assert_eq!("+f38000\n", line);
            sp.write("+OK\n".as_bytes()).unwrap();

            let line = read_line(&mut sp);
            assert_eq!("+s100\n", line);
            sp.write("+OK\n".as_bytes()).unwrap();

            let line = read_line(&mut sp);
            assert_eq!("+s200\n", line);
            sp.write("+OK\n".as_bytes()).unwrap();

            let line = read_line(&mut sp);
            assert_eq!("+s300\n", line);
            sp.write("+OK\n".as_bytes()).unwrap();

            let line = read_line(&mut sp);
            assert_eq!("+send\n", line);
            sp.write("+OK\n".as_bytes()).unwrap();
        });

        let mut hat = Hat::new(
            config,
            &port,
            0.15,
            Box::new(move |message| {
                println!("{:?}", message);
            }),
        );
        hat.open().unwrap();
        hat.transmit("remote1", "button1").unwrap();
    }
}
