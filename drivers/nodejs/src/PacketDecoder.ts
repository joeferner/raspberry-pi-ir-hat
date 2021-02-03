import { Transform, TransformCallback } from 'stream';
import {
    PACKET_TYPE_RESPONSE,
    PACKET_TYPE_RX_IR,
    PACKET_TYPE_TX_IR,
    ResponsePacket,
    RxPacket,
    TxPacket,
} from './packets';

export class PacketDecoder extends Transform {
    _transform(chunk: Buffer, encoding: BufferEncoding, callback: TransformCallback): void {
        try {
            const size = chunk.readUInt16LE(1);
            if (size !== chunk.length) {
                return callback(new Error(`invalid packet size, expected ${chunk.length} found ${size}`));
            } else {
                switch (chunk[0]) {
                    case PACKET_TYPE_TX_IR:
                        this.push(JSON.stringify(PacketDecoder.decodeTxIr(chunk)));
                        break;
                    case PACKET_TYPE_RX_IR:
                        this.push(JSON.stringify(PacketDecoder.decodeRxIr(chunk)));
                        break;
                    case PACKET_TYPE_RESPONSE:
                        this.push(JSON.stringify(PacketDecoder.decodeResponse(chunk)));
                        break;
                    default:
                        return callback(new Error(`invalid packet type: 0x${chunk[0].toString(16)}`));
                }
            }
            callback();
        } catch (err) {
            callback(err);
        }
    }

    private static decodeResponse(chunk: Buffer): ResponsePacket {
        return {
            type: PACKET_TYPE_RESPONSE,
        };
    }

    private static decodeTxIr(chunk: Buffer): TxPacket {
        let offset = 1 /* type */ + 2; /* size */
        const expectedSignalLength =
            (chunk.length - (1 /* type */ + 2 /* size */ + 2 /* frequency */ + 2)) /* signal length */ / 2;
        const frequency = chunk.readUInt16LE(offset);
        offset += 2;
        const signalLength = chunk.readUInt16LE(offset);
        offset += 2;
        if (expectedSignalLength !== signalLength) {
            throw new Error(`invalid signal length, expected ${expectedSignalLength} found ${signalLength}`);
        }
        const signal = [];
        for (let signalIndex = 0; signalIndex < signalLength; signalIndex++) {
            signal.push(chunk.readUInt16LE(offset));
            offset += 2;
        }
        if (offset !== chunk.length) {
            throw new Error('decode error');
        }
        return {
            type: PACKET_TYPE_TX_IR,
            frequency,
            signal,
        };
    }

    private static decodeRxIr(chunk: Buffer): RxPacket {
        const offset = 1 /* type */ + 2; /* size */
        const value = chunk.readUInt32LE(offset);
        return {
            type: PACKET_TYPE_RX_IR,
            value,
        };
    }
}
