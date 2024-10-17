use crate::logger::init_logger;
use anyhow::{anyhow, Context, Result};
use log::debug;

pub struct Config {
}

impl Config {
    pub fn new() -> Result<Config> {
        let log_level = env::var("LOG_LEVEL").unwrap_or("info".to_string());
        let log_level = log::LevelFilter::from_str(&log_level)?;
        init_logger(log_level)?;

        Ok(Config {
        })
    }
}