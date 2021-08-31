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
