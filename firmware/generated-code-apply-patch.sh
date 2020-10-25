#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

patch -p0 --binary --backup --forward -i generated-code.patch
