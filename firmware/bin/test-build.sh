#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${SCRIPT_DIR}/.."

mkdir -p target/test
cd target/test
cmake -DCMAKE_BUILD_TYPE=Debug ../../test || exit 1
make || exit 1
