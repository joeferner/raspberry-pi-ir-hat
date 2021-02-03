import SerialPort from 'serialport';
import Debug from 'debug';
import { RawPacketParser } from './RawPacketParser';
import { NEC_CARRIER_FREQUENCY, Packet, PACKET_TYPE_RESPONSE, PACKET_TYPE_RX_IR, TxPacket } from './packets';
import { PacketDecoder } from './PacketDecoder';
import { PacketEncoder } from './PacketEncoder';

const debug = Debug('raspberry-pi-ir-hat');

export class RaspberryPiIrHat {
    private readonly port: SerialPort;

    constructor(path: string) {
        const options: SerialPort.OpenOptions = {
            autoOpen: false,
            baudRate: 57600,
            parity: 'none',
            dataBits: 8,
            stopBits: 1,
        };
        debug(`creating: ${path}`);
        this.port = new SerialPort(path, options);
        this.port
            .pipe(new RawPacketParser())
            .pipe(new PacketDecoder())
            .on('data', (packet) => {
                try {
                    const p = JSON.parse(packet);
                    this.onPacket(p as Packet);
                } catch (err) {
                    debug('failed to decode packet: ' + packet);
                }
            });
    }

    open(): Promise<void> {
        return new Promise((resolve, reject) => {
            debug(`opening`);
            this.port.open((err) => {
                if (err) {
                    reject(err);
                } else {
                    resolve();
                }
            });
        });
    }

    private write(packet: Buffer): Promise<void> {
        return new Promise((resolve, reject) => {
            this.port.write(packet, (err) => {
                if (err) {
                    reject(err);
                } else {
                    resolve();
                }
            });
        });
    }

    private onPacket(packet: Packet): void {
        switch (packet.type) {
            case PACKET_TYPE_RESPONSE:
                // OK
                break;
            case PACKET_TYPE_RX_IR:
                console.log('on packet', packet);
                break;
        }
    }

    transmit(packet: TxPacket): Promise<void> {
        return this.write(PacketEncoder.encodeIrSignal(packet as TxPacket));
    }
}

export { TxPacket, NEC_CARRIER_FREQUENCY };
