use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::error::Error;
use std::fmt;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;
use std::time::Duration;

use crate::Protocol;

#[derive(Debug, PartialEq, Serialize, Deserialize, Clone)]
pub struct Config {
    remotes: HashMap<String, ConfigRemote>,
}

#[derive(Debug, PartialEq, Serialize, Deserialize, Clone)]
pub struct ConfigRemote {
    buttons: HashMap<String, ConfigButton>,
}

#[derive(Debug, PartialEq, Serialize, Deserialize, Clone)]
pub struct ConfigButton {
    ir_signals: Vec<ConfigIrSignal>,
    /// debounces button press if less than duration
    debounce: Option<u64>,
}

#[derive(Debug, PartialEq, Serialize, Deserialize, Clone, Copy)]
pub struct ConfigIrSignal {
    protocol: Protocol,
    address: u32,
    command: u32,
}

#[derive(Debug)]
pub enum ConfigError {
    WriteError(String, std::io::Error),
    WriteYamlError(String, serde_yaml::Error),
    YamlReadError(String, std::io::Error),
    YamlParseError(String, serde_yaml::Error),
    YamlParseGenericError(String),
}

impl fmt::Display for ConfigError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            ConfigError::WriteError(message, cause) => {
                write!(f, "write error: {}: {}", message, cause)
            }
            ConfigError::WriteYamlError(message, cause) => {
                write!(f, "write yaml error: {}: {}", message, cause)
            }
            ConfigError::YamlReadError(message, err) => {
                write!(f, "yaml read error: {}: {}", message, err)
            }
            ConfigError::YamlParseError(message, err) => {
                write!(f, "yaml parse error: {}: {}", message, err)
            }
            ConfigError::YamlParseGenericError(message) => {
                write!(f, "yaml parse error: {}", message)
            }
        }
    }
}

impl Error for ConfigError {}

impl Config {
    pub fn new() -> Config {
        return Config {
            remotes: HashMap::new(),
        };
    }

    pub fn from_str(yaml: &str) -> Result<Config, ConfigError> {
        let config: Config = serde_yaml::from_str(yaml).map_err(|err| {
            ConfigError::YamlParseError(
                format!("could not read config: contained invalid yaml values"),
                err,
            )
        })?;
        Config::validate_config_yaml(&config)?;
        return Result::Ok(config);
    }

    pub fn from_yaml(yaml: serde_yaml::Value) -> Result<Config, ConfigError> {
        let config: Config = serde_yaml::from_value(yaml).map_err(|err| {
            ConfigError::YamlParseError(
                format!("could not read config: contained invalid yaml values"),
                err,
            )
        })?;
        Config::validate_config_yaml(&config)?;
        return Result::Ok(config);
    }

    pub fn read(filename: &str, create: bool) -> Result<Config, ConfigError> {
        if create && !Path::new(filename).exists() {
            let config = Config::new();
            return Result::Ok(config);
        }

        let file = File::open(filename).map_err(|err| {
            ConfigError::YamlReadError(format!("failed to read file: {}", filename), err)
        })?;
        let reader = BufReader::new(file);
        let config: Config = serde_yaml::from_reader(reader).map_err(|err| {
            ConfigError::YamlParseError(
                format!("could not read file: {}: contained invalid yaml", filename),
                err,
            )
        })?;
        Config::validate_config_yaml(&config)?;
        return Result::Ok(config);
    }

    fn validate_config_yaml(config: &Config) -> Result<(), ConfigError> {
        for (remote_name, remote) in &config.remotes {
            for (button_name, button) in &remote.buttons {
                for ir_signal in &button.ir_signals {
                    let p = ir_signal.protocol;
                    if p == Protocol::Unknown {
                        return Result::Err(ConfigError::YamlParseGenericError(format!(
                            "{}:{} invalid protocol: {}",
                            remote_name, button_name, p as u8
                        )));
                    }
                }
            }
        }
        return Result::Ok(());
    }

    pub fn get_remote_names(&self) -> Vec<String> {
        let mut remote_names = Vec::new();
        for remote_name in self.remotes.keys() {
            remote_names.push(remote_name.clone());
        }
        return remote_names;
    }

    pub fn get_remote(&self, remote_name: &str) -> Option<&ConfigRemote> {
        return self.remotes.get(remote_name);
    }

    pub fn get_button(&self, remote_name: &str, button_name: &str) -> Option<&ConfigButton> {
        return self
            .get_remote(remote_name)
            .and_then(|remote| remote.buttons.get(button_name));
    }

    pub fn to_yaml_string(&self) -> Result<String, serde_yaml::Error> {
        return serde_yaml::to_string(&self);
    }
}

impl ConfigRemote {
    pub fn get_button_names(&self) -> Vec<String> {
        let mut button_names = Vec::new();
        for button_name in self.buttons.keys() {
            button_names.push(button_name.clone());
        }
        return button_names;
    }

    pub fn get_button(&self, button_name: &str) -> Option<&ConfigButton> {
        return self.buttons.get(button_name);
    }
}

impl ConfigButton {
    pub fn get_ir_signals(&self) -> &Vec<ConfigIrSignal> {
        return &self.ir_signals;
    }

    pub fn get_debounce(&self) -> Option<Duration> {
        return match self.debounce {
            Option::None => Option::None,
            Option::Some(v) => Option::Some(Duration::from_millis(v)),
        };
    }
}

impl ConfigIrSignal {
    pub fn get_protocol(&self) -> Protocol {
        return self.protocol;
    }

    pub fn get_address(&self) -> u32 {
        return self.address;
    }

    pub fn get_command(&self) -> u32 {
        return self.command;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_bad_signal_value() {
        Config::from_str(
            r#"
remotes:
  remote1:
    buttons:
      button1:
        signal: ""
"#,
        )
        .expect_err("expected bad signal error");
    }
}
