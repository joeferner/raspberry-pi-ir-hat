use super::MARK_EXCESS_MICROS;

#[allow(dead_code)]
pub enum Endian {
    MSB,
    LSB,
}

pub fn ticks_low(us: u16) -> u16 {
    return us - (us / 2);
}

pub fn ticks_high(us: u16) -> u16 {
    return us + (us / 2);
}

/// Decode pulse distance protocols.
/// The mark (pulse) has constant length, the length of the space determines the bit value.
/// Each bit looks like: MARK + SPACE_1 -> 1
///                 or : MARK + SPACE_0 -> 0
///
/// @param aStartOffset must point to a mark
/// @return true if decoding was successful
pub fn decode_pulse_distance_data(
    signal: &[u16],
    bit_mark_micros: u16,
    one_space_micros: u16,
    zero_space_micros: u16,
    most_significant_bit: Endian,
) -> Option<u32> {
    let mut index: usize = 0;
    let mut decoded_data: u32 = 0;

    match most_significant_bit {
        Endian::MSB => {
            while index < signal.len() - 1 {
                // Check for constant length mark
                if !match_mark(signal[index], bit_mark_micros) {
                    return Option::None;
                }
                index += 1;

                // Check for variable length space indicating a 0 or 1
                if match_space(signal[index], one_space_micros) {
                    decoded_data = (decoded_data << 1) | 1;
                } else if match_space(signal[index], zero_space_micros) {
                    decoded_data = (decoded_data << 1) | 0;
                } else {
                    return Option::None;
                }
                index += 1;
            }
        }
        Endian::LSB => {
            let mut mask = 1;
            while index < signal.len() {
                // Check for constant length mark
                if !match_mark(signal[index], bit_mark_micros) {
                    return Option::None;
                }
                index += 1;

                // Check for variable length space indicating a 0 or 1
                if match_space(signal[index], one_space_micros) {
                    decoded_data |= mask; // set the bit
                } else if match_space(signal[index], zero_space_micros) {
                    // do not set the bit
                } else {
                    return Option::None;
                }
                index += 1;
                mask <<= 1;
            }
        }
    }

    return Option::Some(decoded_data);
}

pub fn match_mark(measured_ticks: u16, match_value_micros: u16) -> bool {
    return (measured_ticks >= ticks_low(match_value_micros + MARK_EXCESS_MICROS))
        && (measured_ticks <= ticks_high(match_value_micros + MARK_EXCESS_MICROS));
}

pub fn match_space(measured_ticks: u16, match_value_micros: u16) -> bool {
    return (measured_ticks >= ticks_low(match_value_micros - MARK_EXCESS_MICROS))
        && (measured_ticks <= ticks_high(match_value_micros - MARK_EXCESS_MICROS));
}
