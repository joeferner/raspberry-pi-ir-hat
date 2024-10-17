use crate::mcp3204::Mcp3204;
use crate::my_error::{MyError, Result};
use crate::utils::stats_list::StatsList;
use crate::utils::time::get_time_millis;
use crate::Message;
use std::sync::mpsc::{self};
use std::thread::{self, JoinHandle};
use std::time::Duration;

const RUNNING_MAX_WINDOW: usize = 100;

#[derive(Debug, Eq, PartialEq, Clone, Copy)]
pub enum State {
    On,
    Off,
}

fn state_changed(prev: &Option<State>, new_state: &State) -> bool {
    return match prev {
        None => true,
        Some(prev_state) => prev_state != new_state,
    };
}

#[derive(Debug)]
pub struct PowerData {
    pub ch0: f64,
    pub ch0_state: State,
    pub ch1: f64,
    pub ch1_state: State,
}

pub struct PowerOptions {
    pub ch0_off: f64,
    pub ch0_on: f64,
    pub ch1_off: f64,
    pub ch1_on: f64,
}

pub struct PowerReceiver {
    thread: JoinHandle<()>,
}

pub struct Power {
    mcp3204: Mcp3204,
    tx: mpsc::Sender<Message>,
    ch0_prev_state: Option<State>,
    ch1_prev_state: Option<State>,
    ch0: StatsList<RUNNING_MAX_WINDOW>,
    ch1: StatsList<RUNNING_MAX_WINDOW>,
    next_log: u128,
}

impl Power {
    pub fn start(
        tx: mpsc::Sender<Message>,
        mcp3204: Mcp3204,
        options: PowerOptions,
    ) -> PowerReceiver {
        let mut power = Power {
            mcp3204,
            tx,
            ch0_prev_state: None,
            ch1_prev_state: None,
            ch0: StatsList::new(),
            ch1: StatsList::new(),
            next_log: 0,
        };

        let thread = thread::spawn(move || loop {
            match power.tick(&options) {
                Ok(()) => thread::sleep(Duration::from_millis(10)),
                Err(err) => {
                    log::error!("failed to get power data {}", err);
                    thread::sleep(Duration::from_secs(5));
                }
            }
        });

        let result = PowerReceiver { thread };
        return result;
    }

    fn calculate_new_state(&self, prev: Option<State>, stddev: f64, on: f64, off: f64) -> State {
        match prev {
            None => {
                if stddev > on {
                    return State::On;
                } else {
                    return State::Off;
                }
            }
            Some(prev_state) => match prev_state {
                State::Off => {
                    if stddev > on {
                        return State::On;
                    } else {
                        return State::Off;
                    }
                }
                State::On => {
                    if stddev < off {
                        return State::Off;
                    } else {
                        return State::On;
                    }
                }
            },
        };
    }

    fn tick(&mut self, options: &PowerOptions) -> Result<()> {
        let ch0 = self.mcp3204.read_single(0)?;
        self.ch0.push(f64::from(ch0));
        let ch1 = self.mcp3204.read_single(1)?;
        self.ch1.push(f64::from(ch1));

        let ch0_stddev = self.ch0.stddev().unwrap_or(0.0);
        let new_ch0_state = self.calculate_new_state(
            self.ch0_prev_state,
            ch0_stddev,
            options.ch0_on,
            options.ch0_off,
        );

        let ch1_stddev = self.ch1.stddev().unwrap_or(0.0);
        let new_ch1_state = self.calculate_new_state(
            self.ch1_prev_state,
            ch1_stddev,
            options.ch1_on,
            options.ch1_off,
        );

        let time = get_time_millis()?;
        if time > self.next_log {
            log::debug!("power: ch0: {:.2}, ch1: {:.2}", ch0_stddev, ch1_stddev);
            self.next_log = time + 5000;
        }

        if state_changed(&self.ch0_prev_state, &new_ch0_state)
            || state_changed(&self.ch1_prev_state, &new_ch1_state)
        {
            let power_data = PowerData {
                ch0: ch0_stddev,
                ch0_state: new_ch0_state,
                ch1: ch1_stddev,
                ch1_state: new_ch1_state,
            };
            self.ch0_prev_state = Some(new_ch0_state);
            self.ch1_prev_state = Some(new_ch1_state);

            self.tx
                .send(Message::PowerData(power_data))
                .map_err(|err| MyError::new(format!("power send error: {}", err)))?;
        }

        return Ok(());
    }
}

impl PowerReceiver {
    pub fn stop(self) -> Result<()> {
        self.thread
            .join()
            .map_err(|err| MyError::new(format!("failed to join thread {:?}", err)))?;
        return Ok(());
    }
}
