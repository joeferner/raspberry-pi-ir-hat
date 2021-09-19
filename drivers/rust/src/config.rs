use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;

#[derive(Deserialize, Serialize, Debug)]
pub struct Config {
    remotes: HashMap<String, String>,
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
        return match serde_json::from_reader(reader) {
            Ok(f) => f,
            Err(err) => {
                return Result::Err(format!("invalid file: {}: {}", filename, err));
            }
        };
    }
}
