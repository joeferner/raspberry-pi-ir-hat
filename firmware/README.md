# STMicroelectronics Documentation

STM32G031F4 - https://github.com/STMicroelectronics/STM32CubeG0

# Development Environment

```
# Cortex-M0+
rustup target add thumbv6m-none-eabi
```

# Pins

| Pin | Pin  | Description       |
| --- | ---- | ----------------- |
|  1  | PB7  | USART1 Rx (Debug) |
|  2  | PC14 | IR Out            |
|  3  | PC15 |                   |
|  4  | Vdd  |                   |
|  5  | Vss  |                   |
|  6  | PF2  | Reset             |
|  7  | PA0  | Current Ref       |
|  8  | PA1  |                   |
|  9  | PA2  | USART2 Tx (Rpi)   |
| 10  | PA3  | USART2 Rx (Rpi)   |
| 11  | PA4  | Current 1         |
| 12  | PA5  | Current 2         |
| 13  | PA6  | IR Rx - TIM3 CH1  |
| 14  | PA7  | IR Activity LED   |
| 15  | PB0  |                   |
| 16  | PA11 |                   |
| 17  | PA12 |                   |
| 18  | PA13 | Sys SWDIO         |
| 19  | PA14 | Sys SWCLK         |
| 20  | PB6  | USART1 Tx (Debug) |

# Links

- stm32g0xx-hal - https://github.com/stm32-rs/stm32g0xx-hal
- setting up debugger in VSCode - https://dev.to/rubberduck/debugging-rust-arm-cortexm-programs-with-visual-studio-code-336h


# STM32CubeMX Project Setup

1. Select MCU STM32G031F4 (https://github.com/STMicroelectronics/STM32CubeG0)
1. Pinout & Configuration
   1. **Programming Port**: Enable "System Core" -> "SYS" -> "Mode" -> "Serial Wire"
   1. **Debug USART**
      1. Enable USART1 "Connectivity" -> "USART1" -> "Mode" -> "Asynchronous"
      1. Baud Rate: 57600
      1. NVIC Settings: Enable global interrupt
   1. **Raspberry Pi USART**: Enable USART2 "Connectivity" -> "USART2" -> "Mode" -> "Asynchronous"
      1. Baud Rate: 57600
      1. NVIC Settings: Enable global interrupt
   1. **IR Tx TIM16**
      - Enable TIM16 "Timers" -> "Channel1" -> "PWM Generation No Output"
      - NVIC Settings: Enable TIM16 global interrupt
   1. **IR Tx TIM17**: Enable TIM17 "Timers" -> "Channel1" -> "PWM Generation No Output"
   1. **IR Tx IRTIM**: Connectivity -> IRTIM -> Activate
   1. **IR Rx LED**: Set PA7 to GPIO_Output and name it `IR_IN_LED`
   1. **IR Rx TIM**
      - Enable TIM3 "Timers" -> "Channel1" -> "Input Capture direct mode"
      - Parameter Settings: "Input Capture Channel 1" -> "Polarity Selection" -> "Both Edges"
      - DMA Settings: TIM3_CH1
        - Mode: Circular
      - NVIC Settings: Enable TIM3 global interrupt 
   1. **Reset Pin**: Set PF2 to GPIO_Input and name it `RESET`
   1. **ADC1**: Enable ADC1
      - Enable IN0 (PA0), IN4 (PA4), IN5 (PA5)
      - Scan Conversion Mode: Enabled
      - Discontinuous Conversion Mode: Enabled
      - SamplingTime Common 1: 12.5 Cycles
      - SamplingTime Common 2: 12.5 Cycles
      - Number of Conversion: 3
      - Rank 1: Channel 0
      - Rank 2: Channel 4
      - Rank 3: Channel 5
      - NVIC Settings -> ADC1 interrupt: Enabled
   1. Enable IWDG
1. Project Manager
   1. Project -> Toolchain/IDE -> Makefile
   1. Advanced Settings -> Driver Selector -> Set all to LL (Low Level)

# Enable STLink

From the RaspberryPi
```
gpio -g mode 18 in
gpio -g mode 17 in
```

OR

```
echo "18" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio18/direction

echo "17" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio17/direction
```
