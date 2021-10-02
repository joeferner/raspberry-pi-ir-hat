use crate::aho_corasick::AhoCorasick;
use crate::ButtonPress;
use crate::Config;
use crate::{RawHat, RawHatMessage};

#[derive(Debug)]
pub enum HatMessage {
    ButtonPress(ButtonPress),
    Error(String),
}

pub struct Hat {
    raw_hat: RawHat,
}

impl Hat {
    pub fn new(
        config: Config,
        port_path: &str,
        tolerance: f32,
        callback: Box<dyn FnMut(HatMessage) + Send>,
    ) -> Hat {
        let mut local_callback = callback;
        let aho_corasick = AhoCorasick::new(&config, tolerance);
        return Hat {
            raw_hat: RawHat::new(
                port_path,
                Box::new(move |msg| {
                    match msg {
                        RawHatMessage::UnknownLine(line) => {
                            local_callback(HatMessage::Error(format!("Unknown line: {}", line)));
                        }
                        RawHatMessage::Signal(signal) => {
                            aho_corasick.append_find(signal);
                            // local_callback(HatMessage::ButtonPress(ButtonPress {
                            //     remote_name: "a".to_string(),
                            //     button_name: "a".to_string(),
                            // }));
                        }
                        RawHatMessage::OkResponse(_) => {}
                        RawHatMessage::ErrResponse(err) => {}
                        RawHatMessage::Error(err) => {
                            local_callback(HatMessage::Error(err));
                        }
                    }
                }),
            ),
        };
    }

    pub fn open(&mut self) -> Result<(), String> {
        return self.raw_hat.open();
    }
}
