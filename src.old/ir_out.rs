use crate::remotes::{Key, Remotes};
use crate::utils::time::get_time_millis;
use crate::{
    lirc::lirc_writer::LircWriter,
    my_error::{MyError, Result},
};
use std::{
    sync::mpsc,
    thread::{self, JoinHandle},
    time::Duration,
};

const IR_OUT_MESSAGE_TIMEOUT_MILLIS: u128 = 250;

pub struct IrOutMessage {
    pub time: Option<u128>,
    pub remote_name: String,
    pub key: Key,
}

impl IrOutMessage {
    pub fn new(remote_name: &str, key: Key, can_timeout: bool) -> Result<Self> {
        let time = if can_timeout {
            Some(get_time_millis()?)
        } else {
            None
        };
        return Ok(IrOutMessage {
            time,
            remote_name: remote_name.to_string(),
            key,
        });
    }
}

pub struct IrOut {
    thread: JoinHandle<()>,
}

impl IrOut {
    pub fn start(
        rx: mpsc::Receiver<IrOutMessage>,
        mut writer: LircWriter,
        remotes: Remotes,
    ) -> Self {
        let mut remotes = Box::new(remotes);
        let thread = thread::spawn(move || loop {
            let mut remotes = remotes.as_mut();
            match Self::tick(&rx, &mut writer, &mut remotes) {
                Ok(()) => {}
                Err(err) => {
                    log::error!("failed to get power data {}", err);
                    thread::sleep(Duration::from_secs(5));
                }
            }
        });

        return IrOut { thread };
    }

    fn tick(
        rx: &mpsc::Receiver<IrOutMessage>,
        writer: &mut LircWriter,
        remotes: &mut Remotes,
    ) -> Result<()> {
        let message = rx.recv()?;

        // check message timeout
        if let Some(time) = message.time {
            let dt = get_time_millis()? - time;
            if dt > IR_OUT_MESSAGE_TIMEOUT_MILLIS {
                log::debug!(
                    "ir out message received too late, expected less than {} found {}",
                    IR_OUT_MESSAGE_TIMEOUT_MILLIS,
                    dt
                );
                return Ok(());
            }
        }

        remotes.send(writer, message.remote_name.as_str(), message.key)?;
        return Ok(());
    }

    pub fn stop(self) -> Result<()> {
        self.thread
            .join()
            .map_err(|err| MyError::new(format!("failed to join thread {:?}", err)))?;
        return Ok(());
    }
}
