#!/usr/bin/env node

import { IrFile, RaspberryPiIrHat, readIrFile, Remote, Signal, writeIrFile } from './';
import yargs from 'yargs';

interface Options {
    file: string;
    port: string;
    remote: string;
    button: string;
    tolerance: number;
    timeout: number;
    debounce?: number;
    minimumSignals?: number;
    numberOfMatchingSignalsByLength?: number;
}

const argv = yargs
    .option('file', {
        alias: 'f',
        type: 'string',
        require: true,
        description: 'File to create/add IR signals to',
    })
    .option('port', {
        alias: 'p',
        type: 'string',
        description: 'Path to serial port',
        default: RaspberryPiIrHat.DEFAULT_OPTIONS.serialPortPath,
    })
    .option('remote', {
        alias: 'r',
        type: 'string',
        require: true,
        description: 'Name of the remote to learn',
    })
    .option('button', {
        alias: 'b',
        type: 'string',
        require: true,
        description: 'Name of the button to learn',
    })
    .option('debounce', {
        type: 'number',
        description: 'Number of milliseconds between consecutive presses of this button',
    })
    .option('tolerance', {
        type: 'number',
        description: 'Signal matching tolerance',
        default: Signal.DEFAULT_OPTIONS.tolerance,
    })
    .option('minimumSignals', {
        type: 'number',
        description: 'Minimum number of signals required',
        default: Signal.DEFAULT_OPTIONS.minimumSignals,
    })
    .option('numberOfMatchingSignalsByLength', {
        type: 'number',
        description: 'Number of matching signals by length',
        default: Signal.DEFAULT_OPTIONS.numberOfMatchingSignalsByLength,
    })
    .option('timeout', {
        type: 'number',
        description: 'Maximum time for an IR signal',
        default: 500,
    }).argv;

async function run(args: Options) {
    const buttons: IrFile = (await readIrFile(args.file)) || {
        remotes: {},
    };
    const signal = new Signal({
        tolerance: args.tolerance,
        minimumSignals: args.minimumSignals,
        numberOfMatchingSignalsByLength: args.numberOfMatchingSignalsByLength,
    });
    let currentSignal: number[] = [];
    let timeout: NodeJS.Timeout | null = null;

    async function endOfSignal() {
        timeout = null;
        try {
            signal.appendSignal(currentSignal);
            console.log('OK');
        } catch (err) {
            console.log(`Button failed: ${err.message}`);
        } finally {
            currentSignal = [];
        }
        if (signal.isComplete) {
            const result = signal.result;
            const remote: Remote = (buttons.remotes[args.remote] = buttons.remotes[args.remote] || {
                buttons: {},
            });
            remote.buttons[args.button] = {
                debounce: args.debounce,
                signal: result.join(','),
            };
            await writeIrFile(args.file, buttons);
            process.exit(0);
        } else {
            console.log(`Press the "${args.button}" on the "${args.remote}" remote again`);
        }
    }

    const hat = new RaspberryPiIrHat({
        serialPortPath: args.port,
        buttons,
    });
    hat.on('rxir', (packet) => {
        currentSignal.push(packet);
        if (timeout === null) {
            timeout = setTimeout(() => {
                endOfSignal().catch((err) => {
                    console.error('failed', err);
                    process.exit(1);
                });
            }, args.timeout);
        }
    });
    await hat.open();

    console.log(`Press the "${args.button}" on the "${args.remote}" remote`);
}

run(argv as Options).catch((err) => {
    console.error('failed to initialize', err);
    process.exit(1);
});
