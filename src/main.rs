use std::process;

use anyhow::Result;
use config::Config;
use lirc::{find_remotes, lirc_reader::LircReader, LircEvent};
use log::{debug, error, info};
use tokio::sync::mpsc;

mod config;
mod ioctl;
mod lirc;
mod logger;
mod rc_devices;

#[tokio::main]
async fn main() -> Result<()> {
    let _config = Config::new()?;
    info!("starting raspberry-pi-ir-hat-network-bridge");

    let (lirc_rx_tx, mut lirc_rx_rx) = mpsc::unbounded_channel::<LircEvent>();

    setup_ir_polarity()?;

    let remotes = find_remotes()?;
    log::debug!("remotes {:#?}", remotes);

    start_reader_loop(&remotes.lirc_rx_device, lirc_rx_tx)?;

    loop {
        let lirc_event = lirc_rx_rx.recv().await;
        debug!("lirc event: {lirc_event:?}");
    }
}

fn start_reader_loop(lirc_rx_device: &str, tx: mpsc::UnboundedSender<LircEvent>) -> Result<()> {
    fn do_read(reader: &mut LircReader, tx: &mpsc::UnboundedSender<LircEvent>) -> Result<()> {
        let data = reader.read()?;
        for item in data {
            tx.send(item)?;
        }
        Ok(())
    }

    let mut reader = LircReader::new(lirc_rx_device)?;
    tokio::spawn(async move {
        loop {
            if let Err(e) = do_read(&mut reader, &tx) {
                error!("failed to read; error = {e}");
                process::exit(1);
            }
        }
    });
    Ok(())
}

#[cfg(not(feature = "mock-rpi"))]
fn setup_ir_polarity() -> Result<()> {
    let gpio = rppal::gpio::Gpio::new()?;

    let mut pin_ir_out_pol = gpio.get(23)?.into_output();
    let mut pin_ir_in_pol = gpio.get(25)?.into_output();
    pin_ir_out_pol.set_low();
    pin_ir_in_pol.set_low();

    Ok(())
}

#[cfg(feature = "mock-rpi")]
fn setup_ir_polarity() -> Result<()> {
    Ok(())
}
