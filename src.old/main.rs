use std::sync::mpsc;
use std::thread;
use std::time::Duration;

use crate::ir_in::IrIn;
use crate::ir_out::{IrOut, IrOutMessage};
use crate::lirc::find_remotes;
use crate::lirc::lirc_reader::LircReader;
use crate::lirc::lirc_writer::LircWriter;
use crate::mcp3204::Mcp3204;
use crate::my_error::Result;
use crate::power::{Power, PowerOptions, State};
use crate::remotes::{Key, Remotes};
use env_logger;
use lirc::LircEvent;
use my_error::MyError;
use power::PowerData;
use remotes::DecodeResult;
use rppal::gpio::Gpio;

mod ioctl;
mod ir_in;
mod ir_out;
mod lirc;
mod mcp3204;
mod my_error;
mod power;
mod rc_devices;
mod remotes;
mod utils;

#[derive(Debug, Eq, PartialEq, Clone, Copy)]
enum Mode {
    Off,
    On,
}

pub enum Message {
    PowerData(PowerData),
    LircEvent(LircEvent),
    Stop,
}

struct Main {
    mode: Mode,
    tx_ir: mpsc::Sender<IrOutMessage>,
    remotes: Remotes,
}

impl Main {
    pub fn run() -> Result<()> {
        let env = env_logger::Env::default();
        env_logger::init_from_env(env);

        let (tx, rx) = mpsc::channel::<Message>();
        let (tx_ir, rx_ir) = mpsc::channel::<IrOutMessage>();

        let mcp3204 = Mcp3204::new()?;
        let power = Power::start(
            tx.clone(),
            mcp3204,
            PowerOptions {
                ch0_off: 5.0,
                ch0_on: 15.0,
                ch1_off: 1.0,
                ch1_on: 20.0,
            },
        );

        let gpio = Gpio::new()?;

        let mut pin_ir_out_pol = gpio.get(23)?.into_output();
        let mut pin_ir_in_pol = gpio.get(25)?.into_output();
        pin_ir_out_pol.set_low();
        pin_ir_in_pol.set_low();

        let remotes = find_remotes()?;
        log::debug!("remotes {:#?}", remotes);
        let reader = LircReader::new(remotes.lirc_rx_device)?;
        let ir_in = IrIn::start(tx, reader);
        let writer = LircWriter::new(remotes.lirc_tx_device)?;
        let ir_out = IrOut::start(rx_ir, writer, Remotes::new());

        let mut main = Main {
            tx_ir,
            mode: Mode::Off,
            remotes: Remotes::new(),
        };

        loop {
            let m = rx.recv()?;
            match m {
                Message::Stop => break,
                Message::LircEvent(lirc_event) => {
                    main.handle_lirc_event(lirc_event)?;
                }
                Message::PowerData(power_data) => main.handle_power_data(power_data)?,
            }
        }

        power.stop()?;
        ir_in.stop()?;
        ir_out.stop()?;

        return Ok(());
    }


    fn handle_power_data(&mut self, power_data: PowerData) -> Result<()> {
        log::debug!("power data {:?}", power_data);
        if power_data.ch0_state == State::On {
            self.set_mode(Mode::On)?;
        } else {
            self.set_mode(Mode::Off)?;
        }
        return Ok(());
    }
}
