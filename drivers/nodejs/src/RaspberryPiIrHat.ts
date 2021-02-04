import { RaspberryPiIrHatRaw } from './RaspberryPiIrHatRaw';
import { AhoCorasick, AhoCorasickButton, AhoCorasickOptions } from './AhoCorasick';
import { RxPacket, TxPacket } from './packets';
import * as events from 'events';
import { IrFile } from './IrFile';
import Debug from 'debug';

const debug = Debug('raspberry-pi-ir-hat:RaspberryPiIrHat');

export interface RaspberryPiIrHatOptions {
    serialPortPath: string;
    buttons: IrFile;
    ahoCorasickOptions?: AhoCorasickOptions;
}

export interface RxButton {
    remoteName: string;
    buttonName: string;
}

export declare interface RaspberryPiIrHat {
    on(event: 'rxir', listener: (packet: RxPacket) => void): this;

    on(event: 'rx', listener: (button: RxButton) => void): this;
}

export class RaspberryPiIrHat extends events.EventEmitter {
    public static readonly DEFAULT_OPTIONS: Partial<RaspberryPiIrHatOptions> = {
        serialPortPath: '/dev/serial0',
        ahoCorasickOptions: AhoCorasick.DEFAULT_OPTIONS,
    };
    private readonly _hat: RaspberryPiIrHatRaw;
    private readonly _ahoCorasick: AhoCorasick;

    constructor(options: RaspberryPiIrHatOptions) {
        super();
        this._hat = new RaspberryPiIrHatRaw(options.serialPortPath);
        this._ahoCorasick = new AhoCorasick(toAhoCorasickButtons(options.buttons), options.ahoCorasickOptions);

        this._hat.on('rxir', (packet) => {
            this._handleRxIrPacket(packet);
        });
    }

    open(): Promise<void> {
        return this._hat.open();
    }

    transmit(packet: TxPacket): Promise<void> {
        return this._hat.transmit(packet);
    }

    private _handleRxIrPacket(packet: RxPacket) {
        this.emit('rxir', packet);

        const result = this._ahoCorasick.appendFind(packet.value);
        if (result) {
            debug(`rx: ${result.remoteName}:${result.buttonName}`);
            this.emit('rx', result);
        }
    }
}

function toAhoCorasickButtons(buttons: IrFile): AhoCorasickButton[] {
    const results: AhoCorasickButton[] = [];
    for (const remoteName of Object.keys(buttons.remotes)) {
        const remote = buttons.remotes[remoteName];
        for (const buttonName of Object.keys(remote.buttons)) {
            const button = remote.buttons[buttonName];
            debug(`adding ${remoteName}:${buttonName}`);
            results.push({
                remoteName,
                buttonName,
                signal: button.signal.split(',').map((b) => parseInt(b)),
            });
        }
    }
    return results;
}
