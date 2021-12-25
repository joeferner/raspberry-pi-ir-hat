pub struct Hertz {
    hertz: u32,
}

impl Hertz {
    pub const fn hertz(hertz: u32) -> Hertz {
        return Hertz { hertz };
    }

    pub const fn kilohertz(kilohertz: u32) -> Hertz {
        return Hertz {
            hertz: kilohertz * 1_000,
        };
    }

    pub const fn megahertz(megahertz: u32) -> Hertz {
        return Hertz {
            hertz: megahertz * 1_000_000,
        };
    }

    pub fn to_hertz(&self) -> u32 {
        return self.hertz;
    }

    pub fn to_megahertz(&self) -> u32 {
        return self.hertz / 1_000_000;
    }
}
