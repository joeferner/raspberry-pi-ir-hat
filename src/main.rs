use std::{process, time::Duration};

use anyhow::Result;
use config::Config;
use lirc::{find_lirc_devices, lirc_reader::LircReader, lirc_writer::LircWriter};
use log::{debug, error, info};
use remotes::{Key, Remotes};
use rumqttc::{AsyncClient, MqttOptions, QoS};
use serde::{Deserialize, Serialize};

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
        mqtt_client.clone(),
    )?;

    mqtt_client
        .subscribe(
            format!("{}/send/#", config.mqtt_topic_prefix),
            QoS::AtMostOnce,
        )
        .await?;

    debug!("starting main event loop");
    let mut state = State {
        lirc_writer: LircWriter::new(lirc_devices.lirc_tx_device)?,
        remotes: Remotes::new(),
    };
    loop {
        match mqtt_event_loop.poll().await {
            Ok(event) => handle_mqtt_event(event, &mut state)?,
            Err(e) => {
                error!("mqtt event loop poll failed; error = {e}");
                process::exit(1);
            }
        }
    }
}

fn handle_mqtt_event(event: rumqttc::Event, state: &mut State) -> Result<()> {
    match event {
        rumqttc::Event::Incoming(packet) => handle_mqtt_incoming_packet(packet, state),
        _ => Ok(()),
    }
}

fn handle_mqtt_incoming_packet(packet: rumqttc::Packet, state: &mut State) -> Result<()> {
    match packet {
        rumqttc::Packet::Publish(publish) => handle_mqtt_incoming_publish_packet(publish, state),
        _ => Ok(()),
    }
}

fn handle_mqtt_incoming_publish_packet(publish: rumqttc::Publish, state: &mut State) -> Result<()> {
    debug!("mqtt publish packet = {:?}", publish);
    if publish.topic.ends_with("/send/raw") {
        handle_mqtt_incoming_publish_packet_raw(publish, state)
    } else if publish.topic.ends_with("/send/encoded") {
        handle_mqtt_incoming_publish_packet_encoded(publish, state)
    } else {
        Ok(())
    }
}

fn handle_mqtt_incoming_publish_packet_raw(
    publish: rumqttc::Publish,
    state: &mut State,
) -> Result<()> {
    match serde_json::from_slice::<SendRaw>(&publish.payload) {
        Ok(send_raw) => {
            state
                .lirc_writer
                .write(send_raw.protocol, send_raw.scan_code)?;
        }
        Err(e) => {
            error!("failed to deserialize raw packet payload; error = {e}");
        }
    }
    Ok(())
}

fn handle_mqtt_incoming_publish_packet_encoded(
    publish: rumqttc::Publish,
    state: &mut State,
) -> Result<()> {
    match serde_json::from_slice::<SendEncoded>(&publish.payload) {
        Ok(send_encoded) => {
            let result = state.remotes.send(
                &mut state.lirc_writer,
                &send_encoded.remote_name,
                send_encoded.key,
            );
            if let Err(e) = result {
                error!("failed to send ir; error = {e}");
            }
        }
        Err(e) => {
            error!("failed to deserialize encoded packet payload; error = {e}");
        }
    }
    Ok(())
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
    let mqtt_raw_receive_topic = format!("{mqtt_topic_prefix}/receive/raw");
    let mqtt_decode_receive_topic = format!("{mqtt_topic_prefix}/receive/decode");
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

struct State {
    lirc_writer: LircWriter,
    remotes: Remotes,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
struct SendRaw {
    protocol: u16,
    scan_code: u64,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
struct SendEncoded {
    remote_name: String,
    key: Key,
}
