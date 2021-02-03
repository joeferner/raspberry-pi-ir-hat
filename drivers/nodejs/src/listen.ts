import { TxPacket, NEC_CARRIER_FREQUENCY, RaspberryPiIrHat } from './';

async function run() {
    const hat = new RaspberryPiIrHat('/dev/serial0');
    await hat.open();

    const packet: TxPacket = {
        frequency: NEC_CARRIER_FREQUENCY,
        signal: [100, 200, 300],
    };
    await hat.transmit(packet);
}

run()
    .then(() => {
        console.log('complete');
    })
    .catch((err) => {
        console.error('failed to initialize', err);
    });
