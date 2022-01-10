#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

nm --print-size --size-sort --radix=d "${SCRIPT_DIR}/../target/firmware.out" | grep -v ' b '
