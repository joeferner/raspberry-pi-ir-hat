import { Readable } from 'stream';
import { PacketDecoder } from './PacketDecoder';
import { TxPacket } from './packets';

test('signal packet', () => {
    return new Promise<void>((resolve, reject) => {
        createStream(Buffer.from([0x02, 11, 0, 0x70, 0x94, 0x02, 0x00, 0x02, 0x01, 0x04, 0x03])).on('data', (d) => {
            const irSignal: TxPacket = JSON.parse(d.toString());
            expect(irSignal.frequency).toBe(38000);
            expect(irSignal.signal).toStrictEqual([0x0102, 0x0304]);
            resolve();
        });
    });
});

function createStream(buffer: Buffer): Readable {
    const readable = new Readable();
    const result = readable.pipe(new PacketDecoder());
    // eslint-disable-next-line
    readable._read = () => {};
    readable.push(buffer);
    readable.push(null);
    return result;
}
