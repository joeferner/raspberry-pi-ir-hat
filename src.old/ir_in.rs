use std::{
    sync::mpsc,
    thread::{self, JoinHandle},
    time::Duration,
};

use crate::{lirc::lirc_reader::LircReader, my_error::MyError, Message};

use crate::my_error::Result;

pub struct IrIn {
    thread: JoinHandle<()>,
}

impl IrIn {
    pub fn start(tx: mpsc::Sender<Message>, mut reader: LircReader) -> Self {
        let thread = thread::spawn(move || loop {
            match Self::tick(&tx, &mut reader) {
                Result::Ok(()) => thread::sleep(Duration::from_millis(10)),
                Result::Err(err) => {
                    log::error!("failed to get power data {}", err);
                    thread::sleep(Duration::from_secs(5));
                }
            }
        });

        return IrIn { thread };
    }

    fn tick(tx: &mpsc::Sender<Message>, reader: &mut LircReader) -> Result<()> {
        let events = reader.read()?;
        for event in events {
            tx.send(Message::LircEvent(event))
                .map_err(|err| MyError::new(format!("ir_in send error: {}", err)))?;
        }
        return Result::Ok(());
    }

    pub fn stop(self) -> Result<()> {
        self.thread
            .join()
            .map_err(|err| MyError::new(format!("failed to join thread {:?}", err)))?;
        return Result::Ok(());
    }
}
