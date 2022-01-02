#include "config.h"
#include "current_sensor.h"
#include "debug.h"
#include "hal/Bus.hpp"
#include "hal/Clocks.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVICHal.hpp"
#include "hal/RCCHal.hpp"
#include "hal/USART.hpp"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

extern hal::Clocks clocks;
extern hal::Bus bus;
extern hal::NVICHal nvic;
extern hal::RCCHal rcc;
extern hal::GPIO resetPin;
extern hal::GPIO irInLedPin;
extern hal::GPIO irOut;
extern hal::GPIO usart1Rx;
extern hal::GPIO usart1Tx;
extern hal::USART usart1;

static const uint32_t HCLK_FREQUENCY = 16000000;

static void setupSystemClock();
static void setupGPIO();
static void setupDMA();
static void setupUSART1();

void setup() {
  bus.enableSyscfgClock();
  bus.enablePwrClock();
  nvic.setPriority(hal::nvic::IRQnType::SysTick_Irq, 3);
  setupSystemClock();
  setupGPIO();
  setupDMA();
  setupUSART1();

  // TODO
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
  clocks.init1msTick(HCLK_FREQUENCY);
  clocks.setSystemCoreClock(HCLK_FREQUENCY);
  rcc.setUSART1ClockSource(hal::rcc::USART1ClockSource::PCLK1);
  rcc.setADCClockSource(hal::rcc::ADCClockSource::SYSCLK);
}

void setupGPIO() {
  clocks.enableGPIOAClock();
  clocks.enableGPIOBClock();
  clocks.enableGPIOFClock();

  irOut.setSpeed(hal::gpio::Speed::Low);
  irOut.setOutputType(hal::gpio::OutputType::PushPull);
  irOut.setPull(hal::gpio::Pull::None);
  irOut.setAlternate(hal::gpio::Alternate::Alt0);
  irOut.setMode(hal::gpio::Mode::Alternate);

  resetPin.setPull(hal::gpio::Pull::None);
  resetPin.setMode(hal::gpio::Mode::Input);

  irInLedPin.resetOutputPin();
  irInLedPin.setSpeed(hal::gpio::Speed::Low);
  irInLedPin.setOutputType(hal::gpio::OutputType::PushPull);
  irInLedPin.setPull(hal::gpio::Pull::None);
  irInLedPin.setMode(hal::gpio::Mode::Output);
}

void setupDMA() {
  clocks.enableDMA1Clock();
  nvic.setPriority(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq);
}

void setupUSART1() {
  clocks.enableUSART1Clock();
  clocks.enableGPIOBClock();

  usart1Rx.setSpeed(hal::gpio::Speed::Low);
  usart1Rx.setOutputType(hal::gpio::OutputType::PushPull);
  usart1Rx.setPull(hal::gpio::Pull::None);
  usart1Rx.setAlternate(hal::gpio::Alternate::Alt0);
  usart1Rx.setMode(hal::gpio::Mode::Alternate);

  usart1Tx.setSpeed(hal::gpio::Speed::Low);
  usart1Tx.setOutputType(hal::gpio::OutputType::PushPull);
  usart1Tx.setPull(hal::gpio::Pull::None);
  usart1Tx.setAlternate(hal::gpio::Alternate::Alt0);
  usart1Tx.setMode(hal::gpio::Mode::Alternate);

  nvic.setPriority(hal::nvic::IRQnType::USART1_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::USART1_Irq);

  usart1.setDataWidth(hal::usart::DataWidth::DataWidth8);
  usart1.setPartity(hal::usart::Parity::None);
  usart1.setStopBits(hal::usart::StopBits::StopBits1);
  usart1.setOverSampling(hal::usart::OverSampling::OverSampling16);
  usart1.setTransferDirection(hal::usart::TransferDirection::TxRx);
  usart1.setHardwareFlowControl(hal::usart::HardwardFlowControl::None);
  usart1.setPrescaler(hal::usart::Prescaler::DIV_1);
  usart1.setBaudRate(rcc, 57600);

  usart1.setTXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.setRXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.disableFIFO();
  usart1.configAsyncMode();

  usart1.enable();
}