use serde::{Deserialize, Serialize};
use serde_json::Error;
use std::collections::HashMap;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;

#[derive(Deserialize, Serialize, Debug)]
pub struct Config {
    pub remotes: HashMap<String, ConfigRemote>,
}

#[derive(Deserialize, Serialize, Debug)]
pub struct ConfigRemote {
    pub buttons: HashMap<String, ConfigButton>,
}

#[derive(Deserialize, Serialize, Debug)]
pub struct ConfigButton {
    /// comma separated list of signals
    pub signal: String,

    /// Number of milliseconds between consecutive presses of this button
    pub debounce: Option<u32>,
}

impl Config {
    pub fn read(filename: &str, create: bool) -> Result<Config, String> {
        if create && !Path::new(filename).exists() {
            let config = Config {
                remotes: HashMap::new(),
            };
            return Result::Ok(config);
        }

        let file = match File::open(filename) {
            Ok(f) => f,
            Err(err) => {
                return Result::Err(format!("invalid file: {}: {}", filename, err));
            }
        };
        let reader = BufReader::new(file);
        let config: Result<Config, Error> = serde_json::from_reader(reader);
        return match config {
            Ok(f) => Result::Ok(f),
            Err(err) => {
                return Result::Err(format!("could not read file: {}: {}", filename, err));
            }
        };
    }

    pub fn set_button(&mut self, remote: &str, button: &str, signal: &str, debounce: Option<u32>) {
        let config_remote =
            self.remotes
                .entry(remote.to_string())
                .or_insert_with(|| ConfigRemote {
                    buttons: HashMap::new(),
                });
        config_remote.buttons.insert(
            button.to_string(),
            ConfigButton {
                debounce,
                signal: signal.to_string(),
            },
        );
    }

    pub fn write(&self, filename: &str) -> Result<(), String> {
        let file = match File::open(filename) {
            Ok(f) => f,
            Err(err) => {
                return Result::Err(format!("invalid file: {}: {}", filename, err));
            }
        };
        return match serde_json::to_writer(file, self) {
            Result::Ok(_) => Result::Ok(()),
            Result::Err(err) => Result::Err(format!("could not write file: {}: {}", filename, err)),
        };
    }
}
