use crate::ir::{
    utils::{decode_pulse_distance_data, match_mark, Endian},
    Protocol, MICROS_PER_TICK,
};

use super::IrSignal;

// Denon publish all their IR codes:
//  https://www.google.co.uk/search?q=DENON+MASTER+IR+Hex+Command+Sheet
//  -> http://assets.denon.com/documentmaster/us/denon%20master%20ir%20hex.xls

// MSB first, no start bit, 5 address + 8 command + 2 frame + 1 stop bit - each frame 2 times
//
const DENON_ADDRESS_BITS: usize = 5;
const DENON_COMMAND_BITS: usize = 8;
const DENON_FRAME_BITS: usize = 2; // 00/10 for 1. frame Denon/Sharp, inverted for autorepeat frame

const DENON_BITS: usize = DENON_ADDRESS_BITS + DENON_COMMAND_BITS + DENON_FRAME_BITS; // 15 - The number of bits in the command
const DENON_UNIT: u16 = 260;

const DENON_BIT_MARK: u16 = DENON_UNIT; // The length of a Bit:Mark
const DENON_ONE_SPACE: u16 = 7 * DENON_UNIT; // 1820 // The length of a Bit:Space for 1's
const DENON_ZERO_SPACE: u16 = 3 * DENON_UNIT; // 780 // The length of a Bit:Space for 0's

const DENON_AUTO_REPEAT_SPACE: u16 = 45000; // Every frame is auto repeated with a space period of 45 ms and the command inverted.
const _DENON_REPEAT_PERIOD: u32 = 110000; // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.

// for old decoder
const DENON_HEADER_MARK: u16 = DENON_UNIT; // The length of the Header:Mark
const _DENON_HEADER_SPACE: u16 = 3 * DENON_UNIT; // 780 // The length of the Header:Space

pub fn decode(signal: &[u16], last_decoded_command: u16) -> Option<IrSignal> {
    // we have no start bit, so check for the exact amount of data bits
    // Check we have the right amount of data (32). The + 2 is for initial gap + stop bit mark
    if signal.len() != (2 * DENON_BITS) + 2 {
        return Option::None;
    }

    // Read the bits in
    let decoded_data_opt = decode_pulse_distance_data(
        &signal[1..],
        DENON_BIT_MARK,
        DENON_ONE_SPACE,
        DENON_ZERO_SPACE,
        Endian::MSB,
    );
    let decoded_data = match decoded_data_opt {
        Option::None => return Option::None,
        Option::Some(v) => v,
    };

    // Check for stop mark
    if !match_mark(signal[(2 * DENON_BITS) + 1], DENON_HEADER_MARK) {
        return Option::None;
    }

    // Success
    let _endian = Endian::MSB;
    let frame_bits: u8 = (decoded_data & 0x03) as u8;
    let mut command: u16 = (decoded_data >> DENON_FRAME_BITS) as u16;
    let address: u16 = (command >> DENON_COMMAND_BITS) as u16;
    command = command & 0xff;
    let mut _repeat_count = 0;
    let mut _auto_repeat = false;
    let mut _parity_failed = false;
    let mut _repeat = false;

    // check for auto repeated inverted command
    // TODO verify MICROS_PER_TICK
    if signal[0] < ((DENON_AUTO_REPEAT_SPACE + (DENON_AUTO_REPEAT_SPACE / 4)) / MICROS_PER_TICK) {
        _repeat_count += 1;
        if frame_bits == 0x3 || frame_bits == 0x1 {
            // We are in the auto repeated frame with the inverted command
            _auto_repeat = true;
            // Check parity of consecutive received commands. There is no parity in one data set.
            let last_command = last_decoded_command;
            if last_command != !command {
                _parity_failed = true;
            }
            // always take non inverted command
            command = last_command;
        }
        if _repeat_count > 1 {
            _repeat = true;
        }
    } else {
        _repeat_count = 0;
    }

    let _number_of_bits = DENON_BITS;
    let protocol = if frame_bits == 1 || frame_bits == 2 {
        Protocol::Sharp
    } else {
        Protocol::Denon
    };
    return Option::Some(IrSignal {
        protocol,
        address,
        command,
    });
}
