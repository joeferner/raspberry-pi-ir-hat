#!/usr/bin/env node

import { RaspberryPiIrHat, readIrFile } from './';
import yargs from 'yargs';

interface Options {
    file: string;
    port: string;
    tolerance: number;
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
    .option('tolerance', {
        type: 'number',
        description: 'Signal matching tolerance',
        default: RaspberryPiIrHat.DEFAULT_OPTIONS.ahoCorasickOptions?.tolerance,
    }).argv;

async function run(args: Options) {
    const buttons = await readIrFile(args.file);
    if (!buttons) {
        throw new Error(`Could not read file: ${args.file}`);
    }
    const hat = new RaspberryPiIrHat({
        serialPortPath: args.port,
        ahoCorasickOptions: {
            tolerance: args.tolerance,
        },
        buttons,
    });
    hat.on('rx', (button) => {
        console.log(`${button.remoteName}:${button.buttonName}`);
    });
    await hat.open();
}

run(argv as Options).catch((err) => {
    console.error('failed to initialize', err);
    process.exit(1);
});
