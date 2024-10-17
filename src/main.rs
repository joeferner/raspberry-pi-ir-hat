use std::{process, time::Duration};

use anyhow::Result;
use config::Config;
use lirc::{find_lirc_devices, lirc_reader::LircReader};
use log::{debug, error, info};
use remotes::Remotes;
use rumqttc::{AsyncClient, MqttOptions, QoS};

mod config;
mod ioctl;
mod lirc;
mod logger;
mod rc_devices;
mod remotes;

#[tokio::main]
async fn main() -> Result<()> {
    let config = Config::new()?;
    info!("starting raspberry-pi-ir-hat-network-bridge");

    setup_ir_polarity()?;

    let lirc_devices = find_lirc_devices()?;
    log::debug!("lirc devices {:#?}", lirc_devices);

    let mut mqtt_options = MqttOptions::new(config.mqtt_id, config.mqtt_host, config.mqtt_port);
    mqtt_options.set_keep_alive(Duration::from_secs(5));

    let (mqtt_client, mut mqtt_event_loop) = AsyncClient::new(mqtt_options, 10);

    start_reader_loop(
        &lirc_devices.lirc_rx_device,
        &config.mqtt_topic_prefix,
        mqtt_client,
    )?;

    debug!("starting main event loop");
    loop {
        match mqtt_event_loop.poll().await {
            Ok(notification) => debug!("mqtt recv = {:?}", notification),
            Err(e) => {
                error!("mqtt event loop poll failed; error = {e}");
                process::exit(1);
            }
        }
    }
}

fn start_reader_loop(
    lirc_rx_device: &str,
    mqtt_topic_prefix: &str,
    mqtt_client: rumqttc::AsyncClient,
) -> Result<()> {
    async fn do_read(
        reader: &mut LircReader,
        mqtt_raw_receive_topic: &str,
        mqtt_decode_receive_topic: &str,
        mqtt_client: &rumqttc::AsyncClient,
        remotes: &mut Remotes,
    ) -> Result<()> {
        let lirc_events = reader.read()?;
        for lirc_event in lirc_events {
            debug!("recv ir event: {lirc_event:?}");
            let payload = serde_json::to_string(&lirc_event)?;
            mqtt_client
                .publish(mqtt_raw_receive_topic, QoS::AtMostOnce, false, payload)
                .await?;

            if let Some(decode_result) = remotes.decode(lirc_event) {
                log::debug!("decode_result {:?}", decode_result);
                let payload = serde_json::to_string(&decode_result)?;
                mqtt_client
                    .publish(mqtt_decode_receive_topic, QoS::AtMostOnce, false, payload)
                    .await?;
            }
        }
        Ok(())
    }

    let mut remotes = Remotes::new();
    let mut reader = LircReader::new(lirc_rx_device)?;
    let mqtt_raw_receive_topic = format!("{mqtt_topic_prefix}/raw/receive");
    let mqtt_decode_receive_topic = format!("{mqtt_topic_prefix}/decode/receive");
    tokio::spawn(async move {
        loop {
            do_read(
                &mut reader,
                &mqtt_raw_receive_topic,
                &mqtt_decode_receive_topic,
                &mqtt_client,
                &mut remotes,
            )
            .await
            .unwrap();
        }
    });
    Ok(())
}

fn setup_ir_polarity() -> Result<()> {
    let gpio = rppal::gpio::Gpio::new()?;

    let mut pin_ir_out_pol = gpio.get(23)?.into_output();
    let mut pin_ir_in_pol = gpio.get(25)?.into_output();
    pin_ir_out_pol.set_low();
    pin_ir_in_pol.set_low();

    Ok(())
}
