use anyhow::Result;
use nix::{
    ioc,
    sys::ioctl::{READ, WRITE},
};
use num_derive::FromPrimitive;
use serde::{Deserialize, Serialize};

pub mod lirc_reader;

#[derive(FromPrimitive, Copy, Clone)]
pub enum LircProtocol {
    Unknown = 0,
    Other = 1,
    Rc5 = 2,
    Rc5x20 = 3,
    Rc5Sz = 4,
    Jvc = 5,
    Sony12 = 6,
    Sony15 = 7,
    Sony20 = 8,
    Nec = 9,
    Necx = 10,
    Nec32 = 11,
    Sanyo = 12,
    Mcir2Kbd = 13,
    Mcir2Mse = 14,
    Rc60 = 15,
    Rc66a20 = 16,
    Rc66a24 = 17,
    Rc66a32 = 18,
    Rc6Mce = 19,
    Sharp = 20,
    Xmp = 21,
    Cec = 22,
    Imon = 23,
    Rcmm12 = 24,
    Rcmm24 = 25,
    Rcmm32 = 26,
    XboxDvd = 27,
}

pub const SCAN_CODE_SIZE: usize = (64 + 16 + 16 + 32 + 64) / 8;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LircEvent {
    pub timestamp: u64,
    pub flags: u16,
    pub rc_protocol: u16,
    pub keycode: u32,
    pub scan_code: u64,
}

#[derive(FromPrimitive)]
pub enum LircIoCtlCommand {
    GetFeatures = ioc!(READ, 'i', 0x00000000, 4) as isize,
    SetSendMode = ioc!(WRITE, 'i', 0x00000011, 4) as isize,
    SetReceiveMode = ioc!(WRITE, 'i', 0x00000012, 4) as isize,
    SetReceiveCarrier = ioc!(WRITE, 'i', 0x00000014, 4) as isize,
    SetReceiveCarrierRange = ioc!(WRITE, 'i', 0x0000001f, 4) as isize,
}

#[derive(FromPrimitive)]
pub enum LircMode {
    Raw = 0x00000001,
    Pulse = 0x00000002,
    Mode2 = 0x00000004,
    ScanCode = 0x00000008,
    LIRCCode = 0x00000010,
}

#[derive(Debug)]
pub struct Remotes {
    pub lirc_rx_device: String,
    pub _lirc_tx_device: String,
}

pub fn find_remotes() -> Result<Remotes> {
    use crate::rc_devices::{enable_all_protocols, find_rc_device_lirc_dev_dir, get_rc_devices};
    use anyhow::anyhow;

    let rc_devices = get_rc_devices()?;
    let lirc_rx_device = find_rc_device_lirc_dev_dir(&rc_devices, "gpio_ir_recv", 0);
    let lirc_rx_device = lirc_rx_device.ok_or(anyhow!("could not find lirc rx device"))?;
    enable_all_protocols(&rc_devices, "gpio_ir_recv")?;

    let lirc_tx_device = find_rc_device_lirc_dev_dir(&rc_devices, "gpio-ir-tx", 0);
    let lirc_tx_device = lirc_tx_device.ok_or(anyhow!("could not find lirc tx device"))?;

    Ok(Remotes {
        lirc_rx_device,
        _lirc_tx_device: lirc_tx_device,
    })
}
