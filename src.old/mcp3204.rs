use crate::my_error::{MyError, Result};
use rppal::spi::{BitOrder, Bus, Mode, SlaveSelect, Spi};

pub struct Mcp3204 {
    spi: Spi,
}

impl Mcp3204 {
    pub fn new() -> Result<Self> {
        let spi = Spi::new(Bus::Spi0, SlaveSelect::Ss0, 20_000, Mode::Mode0)?;
        spi.set_bits_per_word(8)?;
        spi.set_bit_order(BitOrder::MsbFirst)?;
        return Result::Ok(Mcp3204 { spi });
    }

    pub fn read_single(&self, ch: u8) -> Result<u32> {
        if ch > 3 {
            return Result::Err(MyError::new(format!(
                "invalid channel, expected 0,1,2,3 found {}",
                ch
            )));
        }
        let mut read_buffer: [u8; 3] = [0, 0, 0];
        let write_buffer: [u8; 3] = [0b1100_0000 | (ch << 3), 0, 0];
        self.spi.transfer(&mut read_buffer, &write_buffer)?;
        let v0 = (read_buffer[0] as u32) << 16;
        let v1 = (read_buffer[1] as u32) << 8;
        let v2 = read_buffer[2] as u32;
        let value = (v0 | v1 | v2) >> 5;
        return Result::Ok(value);
    }
}
