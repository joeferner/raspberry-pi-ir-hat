import { RaspberryPiIrHatRaw } from './RaspberryPiIrHatRaw';
import { AhoCorasick, AhoCorasickButton, AhoCorasickOptions } from './AhoCorasick';
import * as events from 'events';
import { IrFile } from './IrFile';
import Debug from 'debug';

const debug = Debug('raspberry-pi-ir-hat:RaspberryPiIrHat');

export const NEC_CARRIER_FREQUENCY = 38000;

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
    on(event: 'rxir', listener: (packet: number) => void): this;

    on(event: 'rx', listener: (button: RxButton) => void): this;
}

export class RaspberryPiIrHat extends events.EventEmitter {
    public static readonly DEFAULT_OPTIONS: Partial<RaspberryPiIrHatOptions> = {
        serialPortPath: '/dev/serial0',
        ahoCorasickOptions: AhoCorasick.DEFAULT_OPTIONS,
    };
    private readonly _hat: RaspberryPiIrHatRaw;
    private readonly _ahoCorasick: AhoCorasick;
    private readonly _buttons: IrFile;

    constructor(options: RaspberryPiIrHatOptions) {
        super();
        this._hat = new RaspberryPiIrHatRaw(options.serialPortPath);
        this._buttons = options.buttons;
        this._ahoCorasick = new AhoCorasick(toAhoCorasickButtons(options.buttons), options.ahoCorasickOptions);

        this._hat.on('rxir', (packet: number) => {
            this._handleRxIrPacket(packet);
        });
    }

    open(): Promise<void> {
        return this._hat.open();
    }

    transmit(remoteName: string, buttonName: string): Promise<void> {
        const remote = this._buttons.remotes[remoteName];
        if (!remote) {
            throw new Error(`Could not find remote: ${remoteName}`);
        }
        const button = remote.buttons[buttonName];
        if (!button) {
            throw new Error(`Could not find button ${buttonName} on remote ${remoteName}`);
        }

        const signal = button.signal.split(',').map((b) => parseInt(b));

        return this._hat.transmit(NEC_CARRIER_FREQUENCY, signal);
    }

    private _handleRxIrPacket(packet: number) {
        this.emit('rxir', packet);

        const result = this._ahoCorasick.appendFind(packet);
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
