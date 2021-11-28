pub struct BaudRate {
    bps: u32,
}

impl BaudRate {
    pub fn bps(bps: u32) -> BaudRate {
        return BaudRate { bps };
    }

    pub fn to_bps(&self) -> u32 {
        return self.bps;
    }
}
