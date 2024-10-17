#!/bin/bash
set -eou pipefail
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${DIR}/.."

export RUST_BACKTRACE=1
export LOG_LEVEL=debug
cargo run --features mock-rpi
