#!/usr/bin/env node

import yargs from 'yargs';
import { RaspberryPiIrHat } from './RaspberryPiIrHat';
import { readIrFile } from './IrFile';

interface Options {
    file: string;
    port: string;
    remote: string;
    button: string;
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
    }).argv;

async function run(args: Options) {
    const buttons = await readIrFile(args.file);
    if (!buttons) {
        throw new Error(`Could not read file: ${args.file}`);
    }
    const hat = new RaspberryPiIrHat({
        serialPortPath: args.port,
        buttons,
    });
    await hat.open();
    await hat.transmit(args.remote, args.button);
}

run(argv as Options).catch((err) => {
    console.error('failed to initialize', err);
    process.exit(1);
});
