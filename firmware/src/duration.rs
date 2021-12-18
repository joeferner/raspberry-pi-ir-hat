pub struct Duration {
    microseconds: u32,
}

impl Duration {
    pub const fn milliseconds(t: u32) -> Duration {
        return Duration {
            microseconds: t * 1000,
        };
    }

    pub const fn microseconds(t: u32) -> Duration {
        return Duration { microseconds: t };
    }

    pub fn to_milliseconds(&self) -> u32 {
        return self.microseconds / 1000;
    }

    pub fn to_microseconds(&self) -> u32 {
        return self.microseconds;
    }
}
