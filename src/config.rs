use std::{env, str::FromStr};

use crate::logger::init_logger;
use anyhow::{Context, Result};

pub struct Config {
    pub mqtt_id: String,
    pub mqtt_host: String,
    pub mqtt_port: u16,
    pub mqtt_topic_ir_receive: String,
}

impl Config {
    pub fn new() -> Result<Config> {
        let log_level = env::var("LOG_LEVEL").unwrap_or("info".to_string());
        let log_level = log::LevelFilter::from_str(&log_level)?;
        init_logger(log_level)?;

        let mqtt_id = env::var("MQTT_ID").unwrap_or("pi-ir".to_string());
        let mqtt_host = env::var("MQTT_HOST").context("MQTT_HOST must be set")?;
        let mqtt_port = env::var("MQTT_PORT").unwrap_or("1883".to_string());
        let mqtt_port = mqtt_port
            .parse()
            .with_context(|| format!("invalid MQTT_PORT: {mqtt_port}"))?;

        let mqtt_topic_ir_receive =
            env::var("MQTT_TOPIC_IR_RECEIVE").unwrap_or("home/ir/receive".to_string());

        Ok(Config {
            mqtt_id,
            mqtt_host,
            mqtt_port,
            mqtt_topic_ir_receive,
        })
    }
}
