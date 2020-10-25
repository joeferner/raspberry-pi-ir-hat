#!/bin/bash

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

diff -u Core/Src/main.c.orig Core/Src/main.c > generated-code.patch
diff -u Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c.orig Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c >> generated-code.patch
