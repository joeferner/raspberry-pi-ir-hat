pub struct Hertz {
    hertz: u32,
}

impl Hertz {
    pub fn hertz(hertz: u32) -> Hertz {
        return Hertz { hertz };
    }

    pub fn to_hertz(&self) -> u32 {
        return self.hertz;
    }
}
