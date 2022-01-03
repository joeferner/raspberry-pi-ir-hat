cmake_minimum_required(VERSION 3.15.3)

project(firmware)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SRC_FILES
  src/stm32/system_stm32g0xx.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_adc.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_usart.c
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.c
  src/main.cpp
  src/setup.cpp
  src/ir_rx.cpp
  src/main_it.cpp
  src/ir_tx.cpp
  src/uint8_ring_buffer.cpp
  src/uint32_ring_buffer.cpp
  src/current_sensor.cpp
  src/hal/Bus.cpp
  src/hal/NVIC.cpp
  src/hal/RCC.cpp
  src/hal/Clocks.cpp
  src/hal/System.cpp
  src/hal/IWDG.cpp
  src/startup_stm32g031xx.s
)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${SRC_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DUSE_FULL_LL_DRIVER
  -DSTM32G031xx
  -DHSE_VALUE=8000000
  -DHSE_STARTUP_TIMEOUT=100
  -DLSE_STARTUP_TIMEOUT=5000
  -DLSE_VALUE=32768
  -DEXTERNAL_CLOCK_VALUE=12288000
  -DHSI_VALUE=16000000
  -DLSI_VALUE=32000
  -DVDD_VALUE=3300
  -DPREFETCH_ENABLE=0
  -DINSTRUCTION_CACHE_ENABLE=1
  -DDATA_CACHE_ENABLE=1
)

target_include_directories(${EXECUTABLE} PRIVATE
  src
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Inc
  libs/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx/Include
  libs/STM32CubeG0/Drivers/CMSIS/Include
  libs/etl/include
)

target_compile_options(${EXECUTABLE} PRIVATE
  -mcpu=cortex-m0plus
  -mthumb
  -Wall
  -fdata-sections
  -ffunction-sections
  -fno-exceptions
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<CONFIG:Debug>:-Og -g -gdwarf-2>
)

target_link_options(${EXECUTABLE} PRIVATE
  -T${CMAKE_SOURCE_DIR}/src/STM32G031F4Px_FLASH.ld
  -mcpu=cortex-m0plus
  -mthumb
  -specs=nano.specs
  -lc
  -lm
  -lnosys
  -Wl,-Map=${PROJECT_NAME}.map,--cref
  -Wl,--print-memory-usage,--gc-sections
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
  POST_BUILD
  COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
  COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)

include(cmake/stm32.cmake)
