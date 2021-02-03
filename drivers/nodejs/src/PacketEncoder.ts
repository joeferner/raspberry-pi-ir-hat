import { TxPacket, PACKET_BEGIN, PACKET_END, PACKET_ESCAPE, PACKET_MASK, PACKET_TYPE_TX_IR } from './packets';

export class PacketEncoder {
    static encodeIrSignal(irSignal: TxPacket): Buffer {
        const buffer: Buffer = Buffer.alloc(irSignal.signal.length * 4 + 9);
        let i = buffer.writeUInt8(PACKET_TYPE_TX_IR, 0);
        i = buffer.writeUInt16LE(0, i); // size
        i = buffer.writeUInt32LE(irSignal.frequency, i);
        i = buffer.writeUInt16LE(irSignal.signal.length, i);
        for (const signal of irSignal.signal) {
            i = buffer.writeUInt32LE(signal, i);
        }
        return PacketEncoder.encodeData(buffer);
    }

    private static encodeData(buffer: Buffer): Buffer {
        const size = buffer.length + 1 /* start */ + 1; /* end */
        buffer.writeUInt16LE(size, 1);
        const result = Buffer.alloc((buffer.length * 2 + 2) * 2 /* worst case each byte needs encoding */);
        let inIndex = 0;
        let outIndex = result.writeUInt8(PACKET_BEGIN, 0);
        for (; inIndex < buffer.length; inIndex++) {
            const b = buffer[inIndex];
            if (b === PACKET_BEGIN || b === PACKET_END || b === PACKET_ESCAPE) {
                outIndex = result.writeUInt8(PACKET_ESCAPE, outIndex);
                outIndex = result.writeUInt8(b & PACKET_MASK, outIndex);
            } else {
                outIndex = result.writeUInt8(b, outIndex);
            }
        }
        outIndex = result.writeUInt8(PACKET_END, outIndex);
        return result.slice(0, outIndex);
    }
}
