import Debug from 'debug';

const debug = Debug('raspberry-pi-ir-hat:Signal');

export interface SignalOptions {
    tolerance?: number;
    minimumSignals?: number;
    numberOfMatchingSignalsByLength?: number;
}

export class Signal {
    public static readonly DEFAULT_OPTIONS: Required<SignalOptions> = {
        tolerance: 0.1,
        minimumSignals: 5,
        numberOfMatchingSignalsByLength: 5,
    };
    private readonly _options: Required<SignalOptions>;
    private signals: number[][] = [];
    private _results: number[] | undefined;

    constructor(options: SignalOptions) {
        this._options = { ...Signal.DEFAULT_OPTIONS, ...options };
    }

    appendSignal(signal: number[]): void {
        this._results = undefined;
        this.signals.push(signal.slice());
    }

    private computeResults(): number[] {
        if (this._results) {
            return this._results;
        }

        if (this.signals.length < this._options.minimumSignals) {
            throw new Error(`required ${this._options.minimumSignals} signals, found ${this.signals.length}`);
        }

        const maxMatchingSignalsByLengths = this.getMaxMatchingSignalsByLengths();
        if (maxMatchingSignalsByLengths.length < this._options.numberOfMatchingSignalsByLength) {
            throw new Error(
                `required ${this._options.numberOfMatchingSignalsByLength} matching signals by length, found ${maxMatchingSignalsByLengths.length}`,
            );
        }

        debug(`found ${maxMatchingSignalsByLengths.length} matched length signals`);
        const bestMatches = Signal.findBestMatches(
            maxMatchingSignalsByLengths,
            Math.max(this._options.minimumSignals, this._options.numberOfMatchingSignalsByLength),
        );

        const averageSignal = Signal.computeAverage(bestMatches);
        const worstDiff = Signal.computeWorstDiff(bestMatches, averageSignal);
        if (worstDiff > this._options.tolerance) {
            throw new Error(`signal mismatch - ${(worstDiff * 100).toFixed(1)}%`);
        }

        debug(`valid signal ${averageSignal.join(',')}`);
        this._results = averageSignal;
        return this._results;
    }

    get isComplete(): boolean {
        try {
            this.computeResults();
            return true;
        } catch (err) {
            debug(`results incomplete ${err.message}\n${err.stack}`);
            return false;
        }
    }

    get result(): number[] {
        return this.computeResults();
    }

    private getMaxMatchingSignalsByLengths(): number[][] {
        const signalCountsByLength: { [len: string]: number[][] } = {};
        for (const signal of this.signals) {
            const len = `${signal.length}`;
            if (!(len in signalCountsByLength)) {
                signalCountsByLength[len] = [];
            }
            signalCountsByLength[len].push(signal);
        }

        let maxLength = 0;
        let maxKey: string | undefined = undefined;
        for (const key of Object.keys(signalCountsByLength)) {
            if (signalCountsByLength[key].length > maxLength) {
                maxLength = signalCountsByLength[key].length;
                maxKey = key;
            }
        }
        if (!maxKey) {
            throw new Error('invalid state');
        }
        return signalCountsByLength[maxKey];
    }

    private static computeWorstDiff(signals: number[][], averageSignal: number[]): number {
        let result = 0;
        for (const matchingSignal of signals) {
            for (let i = 0; i < matchingSignal.length; i++) {
                const diff = (matchingSignal[i] - averageSignal[i]) / averageSignal[i];
                if (Math.abs(diff) > result) {
                    result = Math.abs(diff);
                }
            }
        }
        return result;
    }

    private static computeAverage(signals: number[][]) {
        const ret: number[] = [];
        for (let i = 0; i < signals[0].length; i++) {
            let sum = 0;
            for (const s of signals) {
                sum += s[i];
            }
            ret.push(Math.round(sum / signals.length));
        }
        return ret;
    }

    private static findBestMatches(matches: number[][], minNumberToMatch: number): number[][] {
        const combinations = Signal.findCombinations(matches, 0);
        let bestMatch: number[][] = [];
        let bestMatchDiff = Number.MAX_SAFE_INTEGER;
        for (const combination of combinations) {
            if (combination.length < minNumberToMatch) {
                continue;
            }
            const avg = Signal.computeAverage(combination);
            const diff = Signal.computeWorstDiff(combination, avg);
            if (diff < bestMatchDiff) {
                bestMatch = combination;
                bestMatchDiff = diff;
            }
        }
        return bestMatch;
    }

    private static findCombinations(matches: number[][], startIndex: number): number[][][] {
        const results: number[][][] = [];
        for (let i = startIndex; i < matches.length; i++) {
            const nestedResults = Signal.findCombinations(matches, i + 1);
            results.push([matches[i]]);
            for (const nestedResult of nestedResults) {
                results.push([matches[i], ...nestedResult]);
            }
        }
        return results;
    }
}
