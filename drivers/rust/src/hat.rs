use crate::ButtonPress;
use crate::Config;
use crate::RawHat;

#[derive(Debug)]
pub enum HatMessage {
    ButtonPress(ButtonPress),
}

pub struct Hat {
    config: Config,
    tolerance: f32,
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
        return Hat {
            config,
            tolerance,
            raw_hat: RawHat::new(
                port_path,
                Box::new(move |msg| {
                    println!("{:#?}", msg);
                    local_callback(HatMessage::ButtonPress(ButtonPress {
                        remote_name: "a".to_string(),
                        button_name: "a".to_string(),
                    }));
                }),
            ),
        };
    }

    pub fn open(&mut self) -> Result<(), String> {
        return self.raw_hat.open();
    }
}
