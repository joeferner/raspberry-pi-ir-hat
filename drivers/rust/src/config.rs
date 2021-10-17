use serde_json::json;
use std::error::Error;
use std::fmt;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;

#[derive(Debug, Clone)]
pub struct Config {
    data: serde_json::Map<String, serde_json::Value>,
}

#[derive(Debug)]
pub struct ConfigRemote<'a> {
    data: &'a serde_json::Map<String, serde_json::Value>,
}

#[derive(Debug)]
pub struct ConfigButton<'a> {
    /// signal - comma separated list of signals
    /// debounce - Number of milliseconds between consecutive presses of this button
    data: &'a serde_json::Map<String, serde_json::Value>,
}

#[derive(Debug)]
pub enum ConfigError {
    WriteError(String, std::io::Error),
    WriteJsonError(String, serde_json::Error),
    JsonReadError(String, std::io::Error),
    JsonParseError(String, serde_json::Error),
    JsonParseGenericError(String),
}

impl fmt::Display for ConfigError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            ConfigError::WriteError(message, cause) => {
                write!(f, "write error: {}: {}", message, cause)
            }
            ConfigError::WriteJsonError(message, cause) => {
                write!(f, "write json error: {}: {}", message, cause)
            }
            ConfigError::JsonReadError(message, err) => {
                write!(f, "json read error: {}: {}", message, err)
            }
            ConfigError::JsonParseError(message, err) => {
                write!(f, "json parse error: {}: {}", message, err)
            }
            ConfigError::JsonParseGenericError(message) => {
                write!(f, "json parse error: {}", message)
            }
        }
    }
}

impl Error for ConfigError {}

impl Config {
    pub fn from_json(json: serde_json::Value) -> Result<Config, ConfigError> {
        return Config::validate_config_json(json);
    }

    pub fn read(filename: &str, create: bool) -> Result<Config, ConfigError> {
        if create && !Path::new(filename).exists() {
            let data = match json!({
                "remotes": {}
            }) {
                serde_json::Value::Object(obj) => obj,
                _ => panic!("parse failed"),
            };
            let config = Config { data };
            return Result::Ok(config);
        }

        let file = File::open(filename).map_err(|err| {
            ConfigError::JsonReadError(format!("failed to read file: {}", filename), err)
        })?;
        let reader = BufReader::new(file);
        let json = serde_json::from_reader(reader).map_err(|err| {
            ConfigError::JsonParseError(
                format!("could not read file: {}: contained invalid json", filename),
                err,
            )
        })?;
        return Config::validate_config_json(json);
    }

    fn validate_config_json(json: serde_json::Value) -> Result<Config, ConfigError> {
        return match json {
            serde_json::Value::Object(data) => {
                match data.get("remotes") {
                    Option::Some(remotes_value) => {
                        Config::validate_remotes(remotes_value)?;
                    }
                    Option::None => {}
                }
                Result::Ok(Config { data: data })
            }
            _ => Result::Err(ConfigError::JsonParseGenericError(
                "json must be an object".to_string(),
            )),
        };
    }

    fn validate_remotes(remotes_value: &serde_json::Value) -> Result<(), ConfigError> {
        match remotes_value {
            serde_json::Value::Object(remotes) => {
                for remote_name in remotes.keys() {
                    match remotes.get(remote_name).unwrap() {
                        serde_json::Value::Object(remote) => match remote.get("buttons") {
                            Option::Some(buttons_value) => {
                                Config::validate_buttons(remote_name, buttons_value)?;
                            }
                            Option::None => {}
                        },
                        _ => {
                            return Result::Err(ConfigError::JsonParseGenericError(format!(
                                "remote {} must be an object",
                                remote_name
                            )))
                        }
                    }
                }
            }
            _ => {
                return Result::Err(ConfigError::JsonParseGenericError(format!(
                    "remotes must be an object"
                )))
            }
        }
        return Result::Ok(());
    }

    fn validate_buttons(
        remote_name: &str,
        buttons_value: &serde_json::Value,
    ) -> Result<(), ConfigError> {
        match buttons_value {
            serde_json::Value::Object(buttons) => {
                for button_name in buttons.keys() {
                    match buttons.get(button_name).unwrap() {
                        serde_json::Value::Object(button) => {
                            Config::validate_button(remote_name, button_name, &button)?;
                        }
                        _ => {
                            return Result::Err(ConfigError::JsonParseGenericError(format!(
                                "button {}:{} must be an object",
                                remote_name, button_name
                            )))
                        }
                    }
                }
            }
            _ => {
                return Result::Err(ConfigError::JsonParseGenericError(format!(
                    "remote {} buttons must be an object",
                    remote_name
                )))
            }
        }
        return Result::Ok(());
    }

    fn validate_button(
        remote_name: &str,
        button_name: &str,
        button: &serde_json::Map<String, serde_json::Value>,
    ) -> Result<(), ConfigError> {
        match button.get("signal") {
            Option::Some(signal_value) => {
                Config::validate_signal(remote_name, button_name, signal_value)?;
            }
            Option::None => {
                return Result::Err(ConfigError::JsonParseGenericError(format!(
                    "button {}:{} missing signal",
                    remote_name, button_name
                )));
            }
        }
        match button.get("debounce") {
            Option::Some(debounce) => {
                Config::validate_debounce(remote_name, button_name, debounce)?;
            }
            Option::None => {}
        }
        return Result::Ok(());
    }

    fn validate_signal(
        remote_name: &str,
        button_name: &str,
        signal_value: &serde_json::Value,
    ) -> Result<(), ConfigError> {
        match signal_value {
            serde_json::Value::String(signal) => {
                for s in signal.split(",") {
                    match s.trim().parse::<u32>() {
                        Result::Err(err) => {
                            return Result::Err(ConfigError::JsonParseGenericError(format!(
                                "{}:{} signal parse error: {}",
                                remote_name, button_name, err
                            )))
                        }
                        Result::Ok(_) => {}
                    }
                }
            }
            _ => {
                return Result::Err(ConfigError::JsonParseGenericError(format!(
                    "{}:{} signal must be a string",
                    remote_name, button_name
                )));
            }
        }
        return Result::Ok(());
    }

    fn validate_debounce(
        remote_name: &str,
        button_name: &str,
        debounce_value: &serde_json::Value,
    ) -> Result<(), ConfigError> {
        match debounce_value {
            serde_json::Value::Number(_) => {}
            _ => {
                return Result::Err(ConfigError::JsonParseGenericError(format!(
                    "{}:{} debounce must be a number",
                    remote_name, button_name
                )));
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
        debounce: Option<u32>,
    ) {
        let button = self
            .data
            .entry("remotes")
            .or_insert_with(|| json!({}))
            .as_object_mut()
            .expect("remotes is not an object")
            .entry(remote_name)
            .or_insert_with(|| json!({}))
            .as_object_mut()
            .unwrap_or_else(|| panic!("remote {} is not an object", remote_name))
            .entry(button_name)
            .or_insert_with(|| json!({}))
            .as_object_mut()
            .unwrap_or_else(|| panic!("button {}:{} is not an object", remote_name, button_name));

        button.insert(
            "signal".to_string(),
            serde_json::Value::String(signal.to_string()),
        );
        if let Option::Some(d) = debounce {
            button.insert(
                "debounce".to_string(),
                serde_json::Value::Number(serde_json::Number::from(d)),
            );
        }
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
        return match serde_json::to_writer(file, &self.data) {
            Result::Ok(_) => Result::Ok(()),
            Result::Err(err) => Result::Err(ConfigError::WriteJsonError(
                format!("could not write file: {}", filename),
                err,
            )),
        };
    }

    fn get_remotes(&self) -> Option<&serde_json::Map<String, serde_json::Value>> {
        return self.data.get("remotes").and_then(|value| {
            return match value {
                serde_json::Value::Object(obj) => Option::Some(obj),
                _ => panic!("invalid config"),
            };
        });
    }

    pub fn get_remote_names(&self) -> Vec<String> {
        return match self.get_remotes() {
            Option::None => Vec::new(),
            Option::Some(remotes) => {
                let mut result = Vec::new();
                for f in remotes.keys() {
                    result.push(f.to_owned());
                }
                result
            }
        };
    }

    pub fn get_remote(&self, remote_name: &str) -> Option<ConfigRemote> {
        return self.data.get("remotes").and_then(|remotes| {
            return remotes.get(remote_name).and_then(|remote| {
                return remote
                    .as_object()
                    .and_then(|remote| Option::Some(ConfigRemote::new(remote)));
            });
        });
    }

    pub fn get_button(&self, remote_name: &str, button_name: &str) -> Option<ConfigButton> {
        return self.data.get("remotes").and_then(|remotes| {
            return remotes.get(remote_name).and_then(|remote| {
                return remote.as_object().and_then(|remote| {
                    return remote.get("buttons").and_then(|buttons| {
                        return buttons.get(button_name).and_then(|button| {
                            return button.as_object().and_then(|button| {
                                return Option::Some(ConfigButton::new(button));
                            });
                        });
                    });
                });
            });
        });
    }

    pub fn to_json_string(&self) -> Result<String, serde_json::Error> {
        return serde_json::to_string(&self.data);
    }
}

impl<'a> ConfigRemote<'a> {
    pub fn new(data: &'a serde_json::Map<String, serde_json::Value>) -> ConfigRemote<'a> {
        return ConfigRemote { data };
    }

    pub fn get_button_names(&self) -> Vec<String> {
        let mut result = Vec::new();
        for f in self
            .data
            .get("buttons")
            .expect("missing buttons")
            .as_object()
            .expect("buttons should be an object")
            .keys()
        {
            result.push(f.to_owned());
        }
        return result;
    }

    pub fn get_button(&self, button_name: &str) -> Option<ConfigButton> {
        return self
            .data
            .get("buttons")
            .expect("missing buttons")
            .as_object()
            .expect("buttons should be an object")
            .get(button_name)
            .and_then(|value| {
                return match value {
                    serde_json::Value::Object(obj) => Option::Some(ConfigButton::new(obj)),
                    _ => panic!("invalid config"),
                };
            });
    }
}

impl<'a> ConfigButton<'a> {
    pub fn new(data: &'a serde_json::Map<String, serde_json::Value>) -> ConfigButton<'a> {
        return ConfigButton { data };
    }

    pub fn get_signals(&self) -> Vec<u32> {
        return self
            .data
            .get("signal")
            .expect("missing signal")
            .as_str()
            .expect("signal should be a string")
            .split(",")
            .map(|s| {
                s.trim()
                    .parse::<u32>()
                    .map_err(|err| format!("failed to parse {}: {}", s, err))
                    .unwrap()
            })
            .collect();
    }

    pub fn get_json(&self) -> &'a serde_json::Map<String, serde_json::Value> {
        return self.data;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use serde_json::json;
    #[test]
    fn test_bad_signal_value() {
        Config::from_json(json!({
            "remotes": {
                "remote1": {
                    "buttons": {
                        "button1": {
                            "signal": ""
                        }
                    }
                }
            }
        }))
        .expect_err("expected bad signal error");
    }
}
