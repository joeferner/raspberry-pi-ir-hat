#include "config.h"
#include "hal/Bus.hpp"
#include "hal/NVICHal.hpp"
#include "hal/RCCHal.hpp"
#include "current_sensor.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

extern hal::Bus bus;
extern hal::NVICHal nvic;
extern hal::RCCHal rcc;

static void setupSystemClock();

void setup() {
  bus.enableSyscfgClock();
  bus.enablePwrClock();
  nvic.setPriority(hal::nvic::IRQnType::SysTick_Irq, 3);
  setupSystemClock();

  // TODO
  // MX_GPIO_Init();
  // MX_DMA_Init();
  // MX_USART1_UART_Init();
  // MX_USART2_UART_Init();
  // MX_IRTIM_Init();
  // MX_TIM3_Init();
  // MX_TIM16_Init();
  // MX_TIM17_Init();
  // MX_IWDG_Init();
  // MX_ADC1_Init();

  time_setup();
  debug_setup();
  rpi_setup();
  ir_rx_setup();
  ir_tx_setup();
  current_sensor_setup();
  debug_send_string("?READY\n");
  LL_IWDG_Enable(IWDG);
}

void setupSystemClock() {
  rcc.enableHSI();
  rcc.enableLSI();
  rcc.setAHBPrescaler(hal::rcc::AHBPrescaler::DIV_1);
  rcc.setSysClkSource(hal::rcc::SysClkSource::HSI);
  rcc.setAPB1Prescaler(hal::rcc::APB1Prescaler::DIV_1);
  LL_Init1msTick(16000000);
  LL_SetSystemCoreClock(16000000);
  rcc.setUSART1ClockSource(hal::rcc::USART1ClockSource::PCLK1);
  rcc.setADCClockSource(hal::rcc::ADCClockSource::SYSCLK);
}
