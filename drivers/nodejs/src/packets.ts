// 0x7e
export const PACKET_BEGIN = 0b01111110;
// 0x7f
export const PACKET_END = 0b01111111;
// 0x7d
export const PACKET_ESCAPE = 0b01111101;
// 0xdf
export const PACKET_MASK = 0b11011111;

export const PACKET_TYPE_RESPONSE = 0x01;
export const PACKET_TYPE_TX_IR = 0x02;
export const PACKET_TYPE_RX_IR = 0x03;

export const NEC_CARRIER_FREQUENCY = 38000;

// eslint-disable-next-line @typescript-eslint/no-empty-interface
export interface Packet {
    type?: number;
}

// eslint-disable-next-line @typescript-eslint/no-empty-interface
export interface ResponsePacket extends Packet {}

export interface TxPacket extends Packet {
    frequency: number;
    signal: number[];
}

export interface RxPacket extends Packet {
    value: number;
}
