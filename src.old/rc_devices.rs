use crate::my_error::{MyError, Result};
use log;
use std::fs;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone)]
pub struct RcDeviceLirc {
    pub lirc_dir: PathBuf,
    pub dev_name: Option<String>,
}

#[derive(Debug, Clone)]
pub struct RcDeviceInputEvent {
    pub event_dir: PathBuf,
    pub dev_name: Option<String>,
}

#[derive(Debug, Clone)]
pub struct RcDeviceInput {
    pub input_dir: PathBuf,
    pub events: Vec<RcDeviceInputEvent>,
}

#[derive(Debug, Clone)]
pub struct RcDevice {
    pub rc_path: PathBuf,
    pub driver: Option<String>,
    pub inputs: Vec<RcDeviceInput>,
    pub lircs: Vec<RcDeviceLirc>,
}

pub fn get_rc_devices() -> Result<Vec<RcDevice>> {
    let rc_base_dir = "/sys/class/rc";
    let rc_dir = fs::read_dir(rc_base_dir)?;
    let ret: Result<Vec<RcDevice>> = rc_dir
        .map(|rc_path| match rc_path {
            std::result::Result::Ok(rc_path) => {
                let driver = get_uevent_value(&rc_path.path().join("device"), "DRIVER")?;
                let inputs = parse_inputs(&rc_path.path())?;
                let lircs = parse_lircs(&rc_path.path())?;
                return Result::Ok(RcDevice {
                    rc_path: rc_path.path(),
                    driver,
                    inputs,
                    lircs,
                });
            }
            std::result::Result::Err(err) => {
                return Result::Err(err.into());
            }
        })
        .collect();
    return Result::Ok(ret?);
}

fn parse_inputs(rc_path: &Path) -> Result<Vec<RcDeviceInput>> {
    let dirs = fs::read_dir(rc_path)?;
    let inputs = dirs.map(|d| -> Result<Option<RcDeviceInput>> {
        match d {
            std::result::Result::Ok(d) => {
                let file_name = d.file_name().into_string().or_else(|s| {
                    Result::Err(MyError::new(format!("could not convert: {:?}", s)))
                })?;
                if file_name.starts_with("input") {
                    let parsed_input = parse_input(&d.path())?;
                    return Result::Ok(Option::Some(parsed_input));
                } else {
                    return Result::Ok(Option::None);
                }
            }
            std::result::Result::Err(err) => {
                return Result::Err(err.into());
            }
        }
    });
    let inputs: Result<Vec<Option<RcDeviceInput>>> = inputs.collect();
    return Result::Ok(inputs?.iter().filter_map(|d| d.clone()).collect());
}

fn parse_input(dir: &PathBuf) -> Result<RcDeviceInput> {
    let events = parse_input_events(&dir)?;
    return Result::Ok(RcDeviceInput {
        input_dir: dir.to_path_buf(),
        events,
    });
}

fn parse_input_events(dir: &PathBuf) -> Result<Vec<RcDeviceInputEvent>> {
    let dirs = fs::read_dir(dir)?;
    let inputs = dirs.map(|d| -> Result<Option<RcDeviceInputEvent>> {
        match d {
            std::result::Result::Ok(d) => {
                let file_name = d.file_name().into_string().or_else(|s| {
                    Result::Err(MyError::new(format!("could not convert: {:?}", s)))
                })?;
                if file_name.starts_with("input") {
                    let dev_name = get_uevent_value(&d.path().join("uevent"), "DEVNAME")?;
                    return Result::Ok(Option::Some(RcDeviceInputEvent {
                        event_dir: d.path(),
                        dev_name,
                    }));
                } else {
                    return Result::Ok(Option::None);
                }
            }
            std::result::Result::Err(err) => {
                return Result::Err(err.into());
            }
        }
    });
    let inputs: Result<Vec<Option<RcDeviceInputEvent>>> = inputs.collect();
    return Result::Ok(inputs?.iter().filter_map(|d| d.clone()).collect());
}

fn get_uevent_value(dir: &PathBuf, name: &str) -> Result<Option<String>> {
    let path = dir.join("uevent");
    let uevent = fs::read_to_string(path)?;
    let uevent_lines = uevent.split("\n");
    for uevent_line in uevent_lines {
        if let Option::Some(parts) = uevent_line.split_once("=") {
            if parts.0.trim() == name {
                return Result::Ok(Option::Some(parts.1.trim().to_string()));
            }
        }
    }
    return Result::Ok(Option::None);
}

fn parse_lircs(dir: &PathBuf) -> Result<Vec<RcDeviceLirc>> {
    let dirs = fs::read_dir(dir)?;
    let inputs = dirs.map(|d| -> Result<Option<RcDeviceLirc>> {
        match d {
            std::result::Result::Ok(d) => {
                let file_name = d.file_name().into_string().or_else(|s| {
                    Result::Err(MyError::new(format!("could not convert: {:?}", s)))
                })?;
                if file_name.starts_with("lirc") {
                    let dev_name = get_uevent_value(&d.path(), "DEVNAME")?;
                    return Result::Ok(Option::Some(RcDeviceLirc {
                        lirc_dir: d.path(),
                        dev_name,
                    }));
                } else {
                    return Result::Ok(Option::None);
                }
            }
            std::result::Result::Err(err) => {
                return Result::Err(err.into());
            }
        }
    });
    let inputs: Result<Vec<Option<RcDeviceLirc>>> = inputs.collect();
    return Result::Ok(inputs?.iter().filter_map(|d| d.clone()).collect());
}

pub fn find_rc_device_lirc_dev_dir(
    devices: &Vec<RcDevice>,
    driver: &str,
    lirc_index: usize,
) -> Option<String> {
    return devices.iter().find_map(|d| {
        if let Option::Some(d_driver) = &d.driver {
            if d_driver == driver {
                if let Option::Some(lirc) = d.lircs.get(lirc_index) {
                    if let Option::Some(dev_name) = &lirc.dev_name {
                        if let Option::Some(path) = Path::new("/dev").join(dev_name).to_str() {
                            return Option::Some(path.to_string());
                        }
                    }
                }
            }
        }
        return Option::None;
    });
}

pub fn enable_all_protocols(devices: &Vec<RcDevice>, driver: &str) -> Result<()> {
    for device in devices {
        if let Option::Some(d_driver) = &device.driver {
            if *d_driver == driver {
                enable_all_protocols_on_device(device)?;
            }
        }
    }
    return Result::Ok(());
}

pub fn enable_all_protocols_on_device(device: &RcDevice) -> Result<()> {
    log::debug!("enabling all protocols on {:?}", device.rc_path);
    let protocols_file_path = device.rc_path.join("protocols");
    let protocols_file = protocols_file_path
        .to_str()
        .ok_or(MyError::new("Could not create protocol path"))?;
    let protocols_file_content = fs::read_to_string(protocols_file)?;
    let protocols = protocols_file_content.split(" ");
    for protocol in protocols {
        if !protocol.starts_with("[") {
            log::debug!("enabling protocol {}", protocol);
            fs::write(protocols_file, format!("+{}", protocol))?;
        }
    }
    return Result::Ok(());
}
