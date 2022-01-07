#include "globals.hpp"
#include "hal/Bus.hpp"
#include "hal/Clocks.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/IWDG.hpp"
#include "hal/NVIC.hpp"
#include "hal/RCC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "hal/USART.hpp"
#include "main.h"

extern hal::System halSystem;
extern hal::Clocks clocks;
extern hal::Bus bus;
extern hal::NVICHal nvic;
extern hal::RCCHal rcc;

extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin0> currentRefPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin4> current0Pin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin5> current1Pin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin6> irRxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7> irInLedPin;

extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6> usart1TxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7> usart1RxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9> irOutPin;

extern hal::GPIO<hal::gpio::GPIOAddress::GPIOFAddress, hal::gpio::GPIOPin::Pin2> resetPin;

extern hal::USART<hal::usart::USARTAddress::USART1Address> usart1;
extern hal::USART<hal::usart::USARTAddress::USART2Address> usart2;
extern hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5> irRxDmaChannel;
extern hal::Timer<hal::timer::TimerAddress::TIM3Address> irRxTimer;
extern hal::Timer<hal::timer::TimerAddress::TIM17Address> irTxCarrierTimer;
extern hal::Timer<hal::timer::TimerAddress::TIM16Address> irTxSignalTimer;
extern hal::IWDGHal iwdg;

static const uint32_t HCLK_FREQUENCY = 16000000;

static void setupSystemClock();
static void setupGPIO();
static void setupDMA();
static void setupIWDG();

void setup() {
  bus.enableSyscfgClock();
  bus.enablePwrClock();
  setupSystemClock();
  setupGPIO();
  setupDMA();
  setupIWDG();

  debugUsart.initialize(
      clocks,
      nvic,
      rcc,
      usart1,
      usart1TxPin,
      usart1RxPin,
      57600,
      hal::usart::DataWidth::DataWidth8,
      hal::usart::Parity::None,
      hal::usart::StopBits::StopBits1);
  rpiUsart.initialize(
      clocks,
      nvic,
      rcc,
      usart2,
      usart2TxPin,
      usart2RxPin,
      57600,
      hal::usart::DataWidth::DataWidth8,
      hal::usart::Parity::None,
      hal::usart::StopBits::StopBits1);
  irRx.initialize(nvic, clocks, irRxPin, irRxTimer);
  irTx.initialize(clocks, halSystem, nvic);
  currentSensor.initialize(clocks, currentRefPin, current0Pin, current1Pin);
  debugUsart.write("?READY\n");
  iwdg.enable();
}

static void setupSystemClock() {
  nvic.setPriority(hal::nvic::IRQnType::SysTick_Irq, 3);
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

static void setupGPIO() {
  clocks.enableGPIOAClock();
  clocks.enableGPIOBClock();
  clocks.enableGPIOFClock();

  resetPin.setPull(hal::gpio::Pull::None);
  resetPin.setMode(hal::gpio::Mode::Input);
}

static void setupDMA() {
  clocks.enableDMA1Clock();
  nvic.setPriority(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq);
}

static void setupIWDG() {
  iwdg.setPrescaler(hal::iwdg::Prescaler::Prescaler4);
  iwdg.setReloadCounter(4096);
  while (!iwdg.isReady()) {
  }
  iwdg.reloadCounter();
}
