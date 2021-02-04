import Debug from 'debug';
import SerialPort from 'serialport';
import { RawPacketParser } from './RawPacketParser';
import { PacketDecoder } from './PacketDecoder';
import { Packet, PACKET_TYPE_RESPONSE, PACKET_TYPE_RX_IR, RxPacket, TxPacket } from './packets';
import { PacketEncoder } from './PacketEncoder';
import * as events from 'events';

const debug = Debug('raspberry-pi-ir-hat:RaspberryPiIrHatRaw');

export declare interface RaspberryPiIrHatRaw {
    on(event: 'rxir', listener: (packet: RxPacket) => void): this;
}

export class RaspberryPiIrHatRaw extends events.EventEmitter {
    private readonly _port: SerialPort;

    constructor(path: string) {
        super();

        const options: SerialPort.OpenOptions = {
            autoOpen: false,
            baudRate: 57600,
            parity: 'none',
            dataBits: 8,
            stopBits: 1,
        };

        debug(`opening serial port: ${path}`);
        this._port = new SerialPort(path, options);
        this._port
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
            this._port.open((err) => {
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
            this._port.write(packet, (err) => {
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
            case PACKET_TYPE_RX_IR: {
                const p: RxPacket = packet as RxPacket;
                this.emit('rxir', p);
                break;
            }
            default:
                debug(`unhandled packet ${packet.type}`);
                break;
        }
    }

    transmit(packet: TxPacket): Promise<void> {
        return this.write(PacketEncoder.encodeIrSignal(packet as TxPacket));
    }
}
