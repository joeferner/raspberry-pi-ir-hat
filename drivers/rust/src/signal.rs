use itertools::Itertools;
use std::collections::HashMap;

pub struct Signal {
    signals: Vec<Vec<u32>>,
    tolerance: f32,
    minimum_signals: usize,
    number_of_matching_signals_by_length: usize,
}

impl Signal {
    pub fn new(
        tolerance: f32,
        minimum_signals: usize,
        number_of_matching_signals_by_length: usize,
    ) -> Signal {
        return Signal {
            signals: Vec::new(),
            tolerance,
            minimum_signals,
            number_of_matching_signals_by_length,
        };
    }

    pub fn push(&mut self, s: &Vec<u32>) -> Result<String, String> {
        self.signals.push(s.clone());

        if self.signals.len() < self.minimum_signals {
            return Result::Err(format!(
                "minimum number of signals not met. Expected at least {}, found {}",
                self.minimum_signals,
                self.signals.len()
            ));
        }

        let matching_signals_by_lengths = self.get_matching_signals_by_lengths();
        if matching_signals_by_lengths.len() < self.number_of_matching_signals_by_length {
            return Result::Err(format!(
                "minimum number of matching length signals not met. Expected at least {}, found {}",
                self.number_of_matching_signals_by_length,
                matching_signals_by_lengths.len()
            ));
        }

        let best_matches = Signal::find_best_matches(
            &matching_signals_by_lengths,
            usize::max(
                self.minimum_signals,
                self.number_of_matching_signals_by_length,
            ),
        );

        let average_signal = Signal::compute_average(&best_matches);
        let worst_diff = Signal::compute_worst_diff(&best_matches, &average_signal);
        if worst_diff > self.tolerance {
            return Result::Err(format!("signal mismatch - {:.1}%", worst_diff * 100.0));
        }

        let results = average_signal.into_iter().map(|f| f.to_string()).join(",");
        return Result::Ok(results);
    }

    fn get_matching_signals_by_lengths(&self) -> Vec<&Vec<u32>> {
        let mut signal_counts_by_length: HashMap<usize, Vec<&Vec<u32>>> = HashMap::new();
        for signal in &self.signals {
            let entry = signal_counts_by_length
                .entry(signal.len())
                .or_insert(Vec::new());
            entry.push(signal);
        }

        let mut max_length = 0;
        let mut max_key = 0;
        for entry in &signal_counts_by_length {
            if entry.1.len() > max_length {
                max_length = entry.1.len();
                max_key = *entry.0;
            }
        }

        return signal_counts_by_length[&max_key].clone();
    }

    fn find_best_matches(signals: &Vec<&Vec<u32>>, min: usize) -> Vec<Vec<u32>> {
        let combinations = Signal::find_combinations(signals, 0);
        let mut best_match = Vec::new();
        let mut best_match_diff = f32::MAX;
        for combination in combinations {
            if combination.len() < min {
                continue;
            }
            let avg = Signal::compute_average(&combination);
            let diff = Signal::compute_worst_diff(&combination, &avg);
            if diff < best_match_diff {
                best_match = combination;
                best_match_diff = diff;
            }
        }
        return best_match;
    }

    fn find_combinations(signals: &Vec<&Vec<u32>>, start_index: usize) -> Vec<Vec<Vec<u32>>> {
        let mut results: Vec<Vec<Vec<u32>>> = Vec::new();
        for i in start_index..signals.len() {
            let nested_results = Signal::find_combinations(signals, i + 1);
            results.push(vec![signals[i].clone()]);
            for nested_result in nested_results {
                let mut items = vec![signals[i].clone()];
                for c in nested_result {
                    items.push(c);
                }
                results.push(items);
            }
        }
        return results;
    }

    fn compute_average(signals: &Vec<Vec<u32>>) -> Vec<u32> {
        let mut results = Vec::new();
        let len = signals[0].len();
        for i in 0..len {
            let mut sum: u32 = 0;
            for s in signals {
                let si: u32 = (*s)[i];
                sum = sum + si;
            }
            results.push(sum / signals.len() as u32);
        }
        return results;
    }

    fn compute_worst_diff(signals: &Vec<Vec<u32>>, average_signal: &Vec<u32>) -> f32 {
        let mut result = 0.0;
        for signal in signals {
            for i in 0..signal.len() {
                let diff = f32::abs(
                    (signal[i] as f32 - average_signal[i] as f32) / average_signal[i] as f32,
                );
                if diff > result {
                    result = diff;
                }
            }
        }
        return result;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_perfect_match() {
        let mut signal = Signal::new(0.15, 3, 3);

        // signal
        let s = vec![100, 200, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "minimum number of signals not met. Expected at least 3, found 1"
            ))
        );
        // signal
        let s = vec![100, 200, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "minimum number of signals not met. Expected at least 3, found 2"
            ))
        );
        // signal
        let s = vec![100, 200, 300];
        assert_eq!(signal.push(&s), Result::Ok("100,200,300".to_string()));
    }

    #[test]
    fn test_match_within_tolerance() {
        let mut signal = Signal::new(0.15, 3, 3);

        // signal
        let s = vec![100, 200, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "minimum number of signals not met. Expected at least 3, found 1"
            ))
        );
        // signal
        let s = vec![115, 200, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "minimum number of signals not met. Expected at least 3, found 2"
            ))
        );
        // signal - out of tolerance
        let s = vec![100, 150, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "signal mismatch - 18.0%"
            ))
        );
        // signal - out of tolerance
        let s = vec![84, 200, 300];
        assert_eq!(
            signal.push(&s),
            Result::Err(format!(
                "signal mismatch - 16.2%"
            ))
        );
        // signal 3
        let s = vec![100, 200, 300];
        assert_eq!(signal.push(&s), Result::Ok("105,200,300".to_string()));
    }
}
