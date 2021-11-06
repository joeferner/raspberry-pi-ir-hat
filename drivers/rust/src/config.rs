use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::error::Error;
use std::fmt;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;
use std::time::Duration;

#[derive(Debug, PartialEq, Serialize, Deserialize)]
pub struct Config {
    remotes: HashMap<String, ConfigRemote>,
}

#[derive(Debug, PartialEq, Serialize, Deserialize)]
pub struct ConfigRemote {
    buttons: HashMap<String, ConfigButton>,
}

#[derive(Debug, PartialEq, Serialize, Deserialize)]
pub struct ConfigButton {
    /// comma separated list of signals
    signal: String,
    /// Number of milliseconds between consecutive presses of this button
    debounce: Option<Duration>,
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
                for s in button.signal.split(",") {
                    match s.trim().parse::<u32>() {
                        Result::Err(err) => {
                            return Result::Err(ConfigError::YamlParseGenericError(format!(
                                "{}:{} signal parse error: {}",
                                remote_name, button_name, err
                            )))
                        }
                        Result::Ok(_) => {}
                    }
                }
            }
        }
        return Result::Ok(());
    }

    /// # Arguments
    ///
    /// * `remote_name` - name of the remote to set button on
    /// * `button_name` - name of the button to set
    /// * `signal` - comma separated list of signals
    /// * `debounce` - Number of milliseconds between consecutive presses of this button
    pub fn set_button(
        &mut self,
        remote_name: &str,
        button_name: &str,
        signal: &str,
        debounce: Option<Duration>,
    ) {
        let remote = self
            .remotes
            .entry(remote_name.to_string())
            .or_insert_with(|| ConfigRemote {
                buttons: HashMap::new(),
            });
        remote.buttons.insert(
            button_name.to_string(),
            ConfigButton {
                signal: signal.to_string(),
                debounce,
            },
        );
    }

    pub fn write(&self, filename: &str) -> Result<(), ConfigError> {
        let file = match File::open(filename) {
            Ok(f) => f,
            Err(err) => {
                return Result::Err(ConfigError::WriteError(
                    format!("could not open file: {}", filename),
                    err,
                ));
            }
        };
        return match serde_yaml::to_writer(file, &self) {
            Result::Ok(_) => Result::Ok(()),
            Result::Err(err) => Result::Err(ConfigError::WriteYamlError(
                format!("could not write file: {}", filename),
                err,
            )),
        };
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
    pub fn get_signals(&self) -> Vec<u32> {
        return self
            .signal
            .split(",")
            .map(|s| {
                s.trim()
                    .parse::<u32>()
                    .map_err(|err| format!("failed to parse {}: {}", s, err))
                    .unwrap()
            })
            .collect();
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
