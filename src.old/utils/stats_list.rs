use circular_buffer::CircularBuffer;

pub struct StatsList<const N: usize> {
    values: CircularBuffer<N, f64>,
}

impl<const N: usize> StatsList<N> {
    pub fn new() -> Self {
        return StatsList {
            values: CircularBuffer::new(),
        };
    }

    pub fn push(&mut self, value: f64) -> () {
        self.values.push_back(value);
    }

    pub fn mean(&self) -> Option<f64> {
        let sum = self.values.iter().sum::<f64>();
        let count = self.values.len();

        match count {
            positive if positive > 0 => Some(sum / count as f64),
            _ => None,
        }
    }

    pub fn stddev(&self) -> Option<f64> {
        return match (self.mean(), self.values.len()) {
            (Some(data_mean), count) if count > 0 => {
                let variance = self
                    .values
                    .iter()
                    .map(|value| {
                        let diff = data_mean - (*value);
                        return diff * diff;
                    })
                    .sum::<f64>()
                    / count as f64;

                Some(variance.sqrt())
            }
            _ => Option::None,
        };
    }
}
