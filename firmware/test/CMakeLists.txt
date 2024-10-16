cmake_minimum_required(VERSION 3.15.3)

project(firmware_test)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(EXECUTABLE ${PROJECT_NAME}.out)

# googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/6b74da4757a549563d7c37c8fae3e704662a043b.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  ${EXECUTABLE}
  ../src/ir/Decoder.cpp
  ../src/ir/Encoder.cpp
  ../src/ir/ProtocolDecoder.cpp
  ../src/ir/ProtocolEncoder.cpp
  ../src/ir/Denon.cpp
  ../src/ir/Denon_test.cpp
  ../src/ir/NEC.cpp
  ../src/ir/NEC_test.cpp
  ../src/test/test.cpp
)

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
  -DUSE_FULL_ASSERT=1
  -DTEST
)

target_compile_options(${EXECUTABLE} PRIVATE
  -O0
  -Wall
)

target_include_directories(${EXECUTABLE} PRIVATE
  ../src
  ../src/stm32
  ../src/ir
  ../src/test
  ../libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Inc
  ../libs/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx/Include
  ../libs/STM32CubeG0/Drivers/CMSIS/Include
  ../libs/etl/include
)

target_link_libraries(
  ${EXECUTABLE}
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(${EXECUTABLE})
