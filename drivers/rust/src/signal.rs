pub struct Signal {}

impl Signal {
    pub fn new(
        tolerance: f32,
        minimumSignals: u32,
        numberOfMatchingSignalsByLength: u32,
    ) -> Signal {
        return Signal {};
    }

    pub fn push(&mut self, s: &Vec<u32>) -> Result<bool, String> {
        return Result::Ok(false);
    }

    pub fn get_result(&self) -> String {
        return "".to_string();
    }
}
