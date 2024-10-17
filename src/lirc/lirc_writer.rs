use anyhow::Result;

use crate::ioctl;
use std::{fs::File, io::Write, os::fd::AsRawFd};

use super::{LircIoCtlCommand, LircMode, SCAN_CODE_SIZE};

pub struct LircWriter {
    file: File,
}

impl LircWriter {
    pub fn new(lirc_device: String) -> Result<Self> {
        let file = File::options().read(false).write(true).open(lirc_device)?;
        ioctl::write_u32(
            file.as_raw_fd(),
            LircIoCtlCommand::SetSendMode as u32,
            LircMode::ScanCode as u32,
        )?;
        Ok(LircWriter { file })
    }

    pub fn write(&mut self, protocol: u16, scan_code: u64) -> Result<()> {
        log::debug!(
            "writing (protocol: {}, scan_code: 0x{:#x})",
            protocol,
            scan_code
        );
        ioctl::write_u32(
            self.file.as_raw_fd(),
            LircIoCtlCommand::SetSendMode as u32,
            LircMode::ScanCode as u32,
        )?;
        let mut buf = [0; SCAN_CODE_SIZE];

        let protocol_bytes = protocol.to_le_bytes();
        buf[10..12].copy_from_slice(&protocol_bytes);

        let scan_code_bytes = scan_code.to_le_bytes();
        buf[16..24].copy_from_slice(&scan_code_bytes);

        self.file.write_all(&buf)?;
        Ok(())
    }
}
