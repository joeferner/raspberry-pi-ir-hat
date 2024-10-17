use std::sync::Arc;

use anyhow::Result;
use config::Config;
use log::info;

mod config;
mod logger;

#[tokio::main]
async fn main() -> Result<()> {
    let config = Config::new()?;
    info!("starting raspberry-pi-ir-hat-network-bridge");

    Ok(())
}
