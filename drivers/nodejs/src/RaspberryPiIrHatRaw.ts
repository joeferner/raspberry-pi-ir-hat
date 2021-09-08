import Debug from 'debug';
import SerialPort from 'serialport';
import Readline from '@serialport/parser-readline';
import * as events from 'events';

const debug = Debug('raspberry-pi-ir-hat:RaspberryPiIrHatRaw');

export declare interface RaspberryPiIrHatRaw {
    on(event: 'rxir', listener: (packet: number) => void): this;
}

export class RaspberryPiIrHatRaw extends events.EventEmitter {
    private readonly _port: SerialPort;
    private _ack?: (err?: Error) => void;

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
        this._port.pipe(new Readline({ delimiter: '\n' })).on('data', (packet: string) => {
            try {
                packet = packet.trim();
                debug(`read: ${packet}`);
                if (packet.startsWith('!s')) {
                    this.emit('rxir', parseInt(packet.substr('!s'.length)));
                } else if (packet.startsWith('+OK')) {
                    if (this._ack) {
                        this._ack();
                    }
                    this.emit('ack');
                } else {
                    throw new Error(`unhandled packet: ${packet}`);
                }
            } catch (err) {
                debug(`failed to decode packet: ${packet}`, err);
                if (this._ack) {
                    this._ack(err);
                }
                this.emit('error', err);
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

    private write(packet: string): Promise<void> {
        return new Promise((resolve, reject) => {
            debug(`write: ${packet}`);
            this._port.write(`${packet}\n`, (err) => {
                if (err) {
                    reject(err);
                } else {
                    resolve();
                }
            });
        });
    }

    private waitForAck(): Promise<void> {
        return new Promise((resolve, reject) => {
            const t = setTimeout(() => {
                reject(new Error('timeout'));
            }, 1000);
            this._ack = (err?: Error) => {
                clearTimeout(t);
                if (err) {
                    reject(err);
                } else {
                    resolve();
                }
            };
        });
    }

    private async writeAndWaitForAck(packet: string): Promise<void> {
        await this.write(packet);
        await this.waitForAck();
    }

    async transmit(frequency: number, signal: number[]): Promise<void> {
        await this.writeAndWaitForAck(`+f${frequency}`);
        for (let i = 0; i < signal.length; i += 2) {
            await this.writeAndWaitForAck(`+s${signal[i] || 0},${signal[i + 1] || 0}`);
        }
        await this.writeAndWaitForAck('+send');
    }
}
