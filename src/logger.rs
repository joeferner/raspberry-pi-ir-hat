use anyhow::{Context, Result};
use log4rs::{
    append::console::ConsoleAppender,
    config::{Appender, Root},
};

pub fn init_logger(log_level: log::LevelFilter) -> Result<()> {
    let stdout = ConsoleAppender::builder().build();
    let config = log4rs::Config::builder()
        .appender(Appender::builder().build("stdout", Box::new(stdout)))
        .build(Root::builder().appender("stdout").build(log_level))
        .unwrap();
    log4rs::init_config(config).context("failed to initialize default logger")?;

    Ok(())
}
