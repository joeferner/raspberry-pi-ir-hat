pub struct Hertz {
    hertz: u32,
}

impl Hertz {
    pub fn hertz(hertz: u32) -> Hertz {
        return Hertz { hertz };
    }

    pub fn kilohertz(kilohertz: u32) -> Hertz {
        return Hertz {
            hertz: kilohertz * 1_000,
        };
    }

    pub fn megahertz(megahertz: u32) -> Hertz {
        return Hertz {
            hertz: megahertz * 1_000_000,
        };
    }

    pub fn to_hertz(&self) -> u32 {
        return self.hertz;
    }
}
