use anyhow::Result;
use config::Config;
use lirc::{find_remotes, lirc_reader::LircReader};
use log::info;

mod config;
mod logger;
mod lirc;
mod rc_devices;
mod ioctl;

#[tokio::main]
async fn main() -> Result<()> {
    let _config = Config::new()?;
    info!("starting raspberry-pi-ir-hat-network-bridge");

    setup_ir_polarity()?;

    let remotes = find_remotes()?;
    log::debug!("remotes {:#?}", remotes);
    let _reader = LircReader::new(remotes.lirc_rx_device)?;
    
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
