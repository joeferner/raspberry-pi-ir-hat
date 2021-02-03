import { Transform, TransformCallback } from 'stream';
import { PACKET_BEGIN, PACKET_END, PACKET_ESCAPE, PACKET_MASK } from './packets';

export class RawPacketParser extends Transform {
    private buffer: Buffer;

    constructor() {
        super();
        this.buffer = Buffer.alloc(0);
    }

    _transform(chunk: any, encoding: BufferEncoding, callback: TransformCallback) {
        this.buffer = Buffer.concat([this.buffer, chunk]);
        do {
            const b = this.buffer.readUInt8(0);
            if (b === PACKET_BEGIN) {
                const endIndex = this.findEndIndex();
                if (endIndex) {
                    const packet = Buffer.alloc(endIndex + 1);
                    let inIndex, outIndex;
                    let inEscape = false;
                    for (inIndex = 1, outIndex = 0; inIndex < endIndex; inIndex++) {
                        const b = this.buffer[inIndex];
                        if (b === PACKET_ESCAPE) {
                            inEscape = true;
                        } else if (inEscape) {
                            inEscape = false;
                            packet[outIndex++] = b | ~PACKET_MASK;
                        } else {
                            packet[outIndex++] = b;
                        }
                    }
                    this.push(packet.slice(0, outIndex));
                    this.buffer = this.buffer.slice(endIndex + 1);
                }
                break;
            } else {
                this.buffer = this.buffer.slice(1);
            }
        } while (this.buffer.length > 0);
        if (this.buffer.length === 0) {
            this.buffer = Buffer.alloc(0);
        }
        callback();
    }

    private findEndIndex(): number | undefined {
        for (let i = 0; i < this.buffer.length; i++) {
            if (this.buffer.readUInt8(i) === PACKET_END) {
                return i;
            }
        }
        return undefined;
    }
}
