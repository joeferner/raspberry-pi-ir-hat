use anyhow::Result;
use log::debug;

use super::LircEvent;

use super::SCAN_CODE_SIZE;
use super::{LircIoCtlCommand, LircMode};
use crate::ioctl;
use nix::fcntl::{self, OFlag};
use nix::sys::stat::Mode;
use nix::unistd;
use std::os::unix::io::RawFd;

pub struct LircReader {
    fd: RawFd,
}

impl LircReader {
    pub fn new(lirc_device: &str) -> Result<Self> {
        debug!("opening: {}", lirc_device);
        let fd = fcntl::open(lirc_device, OFlag::O_RDONLY, Mode::empty())?;
        ioctl::write_u32(
            fd,
            LircIoCtlCommand::SetReceiveMode as u32,
            LircMode::ScanCode as u32,
        )?;
        Ok(LircReader { fd })
    }

    pub fn read(&mut self) -> Result<Vec<LircEvent>> {
        let mut buf: [u8; SCAN_CODE_SIZE * 64] = [0; SCAN_CODE_SIZE * 64];
        let ret = unistd::read(self.fd, &mut buf)?;
        let mut events = vec![];
        for offset in (0..ret).step_by(SCAN_CODE_SIZE) {
            let event = LircEvent {
                timestamp: u64::from_le_bytes(buf[offset..(offset + 8)].try_into()?),
                flags: u16::from_le_bytes(buf[(offset + 8)..(offset + 10)].try_into()?),
                rc_protocol: u16::from_le_bytes(buf[(offset + 10)..(offset + 12)].try_into()?),
                keycode: u32::from_le_bytes(buf[(offset + 12)..(offset + 16)].try_into()?),
                scan_code: u64::from_le_bytes(buf[(offset + 16)..(offset + 24)].try_into()?),
            };
            events.push(event);
        }
        Ok(events)
    }
}
