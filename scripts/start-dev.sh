#!/bin/bash
set -eou pipefail
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${DIR}/.."

export RUST_BACKTRACE=1
export LOG_LEVEL=debug
sudo chmod a+w $(find /sys/ 2>/dev/null | grep protocols)
sudo chmod a+w $(find /dev/lirc* 2>/dev/null)
cargo run
