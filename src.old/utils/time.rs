use std::time::{SystemTime, UNIX_EPOCH};

use crate::my_error::Result;

pub fn get_time_millis() -> Result<u128> {
    let time = SystemTime::now().duration_since(UNIX_EPOCH)?.as_millis();
    return Result::Ok(time);
}
