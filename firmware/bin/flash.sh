#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${SCRIPT_DIR}/.."

addr=$(cat src/STM32G031F4Px_FLASH.ld | grep 'FLASH.*ORIGIN.*LENGTH' | sed -n 's|^.*ORIGIN = \([^,]*\),.*$|\1|p')
echo "Flashing to ${addr}"
st-flash write target/firmware.bin $addr
