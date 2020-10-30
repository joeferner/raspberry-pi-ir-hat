#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

rm Core/Src/stm32f0xx_it.c
rm Core/Src/main.c
