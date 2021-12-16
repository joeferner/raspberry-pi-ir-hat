pub enum Duration {
    Microseconds(u32),
    Milliseconds(u32),
}

impl Duration {
    pub const fn from_millis(t: u32) -> Duration {
        return Duration::Milliseconds(t);
    }

    pub fn to_milliseconds(&self) -> u32 {
        return match self {
            &Duration::Microseconds(t) => t * 1000,
            &Duration::Milliseconds(t) => t,
        };
    }
}
