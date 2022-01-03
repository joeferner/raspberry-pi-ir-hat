#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${SCRIPT_DIR}/.."

mkdir -p target
cd target
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
