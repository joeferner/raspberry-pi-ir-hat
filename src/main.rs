use std::{process, time::Duration};

use anyhow::Result;
use config::Config;
use lirc::{find_remotes, lirc_reader::LircReader};
use log::{debug, error, info};
use rumqttc::{AsyncClient, MqttOptions, QoS};

mod config;
mod ioctl;
mod lirc;
mod logger;
mod rc_devices;

#[tokio::main]
async fn main() -> Result<()> {
    let config = Config::new()?;
    info!("starting raspberry-pi-ir-hat-network-bridge");

    setup_ir_polarity()?;

    let remotes = find_remotes()?;
    log::debug!("remotes {:#?}", remotes);

    let mut mqtt_options = MqttOptions::new(config.mqtt_id, config.mqtt_host, config.mqtt_port);
    mqtt_options.set_keep_alive(Duration::from_secs(5));

    let (client, mut event_loop) = AsyncClient::new(mqtt_options, 10);

    start_reader_loop(&remotes.lirc_rx_device, &config.mqtt_topic_ir_receive, client)?;

    while let Ok(notification) = event_loop.poll().await {
        debug!("mqtt recv = {:?}", notification);
    }

    Ok(())
}

fn start_reader_loop(
    lirc_rx_device: &str,
    mqtt_topic: &str,
    client: rumqttc::AsyncClient,
) -> Result<()> {
    async fn do_read(
        reader: &mut LircReader,
        mqtt_topic: &str,
        client: &rumqttc::AsyncClient,
    ) -> Result<()> {
        let data = reader.read()?;
        for item in data {
            debug!("recv ir event: {item:?}");
            let payload = serde_json::to_string(&item)?;
            client
                .publish(mqtt_topic, QoS::AtLeastOnce, false, payload)
                .await?;
        }
        Ok(())
    }

    let mqtt_topic = mqtt_topic.to_string();
    let mut reader = LircReader::new(lirc_rx_device)?;
    tokio::spawn(async move {
        loop {
            if let Err(e) = do_read(&mut reader, &mqtt_topic, &client).await {
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
