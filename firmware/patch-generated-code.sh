#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

echo "" > Core/Src/stm32f0xx_it.c
echo "" > Core/Src/main.c
