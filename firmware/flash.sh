#!/bin/bash

addr=$(cat memory.x | grep 'FLASH.*ORIGIN.*LENGTH' | sed -n 's|^.*ORIGIN = \([^,]*\),.*$|\1|p')
echo "Flashing to ${addr}"
st-flash write target/thumbv6m-none-eabi/release/firmware.bin $addr
