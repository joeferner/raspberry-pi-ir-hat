import { Readable } from 'stream';
import { RawPacketParser } from './RawPacketParser';

test('simple packet', () => {
    return new Promise<void>((resolve, reject) => {
        createStream([Buffer.from([0, 1, 2, 3, 0x7e, 4, 5, 6, 0x7f])]).on('data', (d) => {
            expect([...d]).toStrictEqual([4, 5, 6]);
            resolve();
        });
    });
});

test('packet with encoded value', () => {
    return new Promise<void>((resolve, reject) => {
        createStream([Buffer.from([0x7e, 1, 0x7d, 0x5f, 2, 0x7f])]).on('data', (d) => {
            expect([...d]).toStrictEqual([1, 0x7f, 2]);
            resolve();
        });
    });
});

function createStream(buffers: Buffer[]): Readable {
    const readable = new Readable();
    const result = readable.pipe(new RawPacketParser());
    // eslint-disable-next-line
    readable._read = () => {};
    for (const buffer of buffers) {
        readable.push(buffer);
    }
    readable.push(null);
    return result;
}
