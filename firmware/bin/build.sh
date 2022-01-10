#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${SCRIPT_DIR}/.."

MODE=${1:-Debug}

if [ ${MODE} != "Debug" ] && [ ${MODE} != "Release" ]; then
    echo "mode must be 'Debug' or 'Release'"
    exit 1
fi

if [ -f target/MODE ]; then
    OLD_MODE=$(cat target/MODE)
    if [ ${MODE} != ${OLD_MODE} ]; then
        rm -rf target
    fi
fi

mkdir -p target
cd target
echo ${MODE} > MODE
if [ ! -f Makefile ]; then
    cmake -DCMAKE_BUILD_TYPE=${MODE} -DCMAKE_TOOLCHAIN_FILE=../cmake/stm32.cmake .. || exit 1
fi
make
