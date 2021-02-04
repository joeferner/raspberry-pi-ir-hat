import fs from 'fs';

export interface Button {
    signal: string;
    debounce?: number;
}

export interface Remote {
    buttons: { [name: string]: Button };
}

export interface IrFile {
    remotes: { [name: string]: Remote };
}

export async function readIrFile(path: string): Promise<IrFile | undefined> {
    try {
        const data = await fs.promises.readFile(path, 'utf8');
        const rawData = JSON.parse(data);
        if (!('remotes' in rawData)) {
            rawData.remotes = {};
        }
        return rawData;
    } catch (err) {
        if (err.code === 'ENOENT') {
            return undefined;
        }
        throw err;
    }
}

export async function writeIrFile(path: string, file: IrFile): Promise<void> {
    return await fs.promises.writeFile(path, JSON.stringify(file, null, 2));
}
