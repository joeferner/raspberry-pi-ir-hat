#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

echo "" > Core/Src/stm32f0xx_hal_msp.c
echo "" > Core/Src/stm32f0xx_it.c
echo "" > Core/Src/main.c

for f in $(find Drivers | grep hal); do
  echo "" > $f
done
