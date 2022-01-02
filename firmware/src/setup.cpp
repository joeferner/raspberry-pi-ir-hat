#include "config.h"
#include "current_sensor.h"
#include "debug.h"
#include "hal/Bus.hpp"
#include "hal/Clocks.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVICHal.hpp"
#include "hal/RCCHal.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "hal/USART.hpp"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

extern hal::System halSystem;
extern hal::Clocks clocks;
extern hal::Bus bus;
extern hal::NVICHal nvic;
extern hal::RCCHal rcc;
extern hal::GPIO resetPin;
extern hal::GPIO irInLedPin;
extern hal::GPIO irOutPin;
extern hal::GPIO usart1RxPin;
extern hal::GPIO usart1TxPin;
extern hal::USART usart1;
extern hal::GPIO usart2RxPin;
extern hal::GPIO usart2TxPin;
extern hal::USART usart2;
extern hal::GPIO irRxPin;
extern hal::DMAChannel irRxDmaChannel;
extern hal::Timer irRxTimer;

static const uint32_t HCLK_FREQUENCY = 16000000;

static void setupSystemClock();
static void setupGPIO();
static void setupDMA();
static void setupUSART1();
static void setupUSART2();
static void setupIRTIM();
static void setupTIM3();

void setup() {
  bus.enableSyscfgClock();
  bus.enablePwrClock();
  nvic.setPriority(hal::nvic::IRQnType::SysTick_Irq, 3);
  setupSystemClock();
  setupGPIO();
  setupDMA();
  setupUSART1();
  setupUSART2();
  setupIRTIM();
  setupTIM3();

  // TODO
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

  irOutPin.setSpeed(hal::gpio::Speed::Low);
  irOutPin.setOutputType(hal::gpio::OutputType::PushPull);
  irOutPin.setPull(hal::gpio::Pull::None);
  irOutPin.setAlternate(hal::gpio::Alternate::Alt0);
  irOutPin.setMode(hal::gpio::Mode::Alternate);

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

  usart1RxPin.setSpeed(hal::gpio::Speed::Low);
  usart1RxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart1RxPin.setPull(hal::gpio::Pull::None);
  usart1RxPin.setAlternate(hal::gpio::Alternate::Alt0);
  usart1RxPin.setMode(hal::gpio::Mode::Alternate);

  usart1TxPin.setSpeed(hal::gpio::Speed::Low);
  usart1TxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart1TxPin.setPull(hal::gpio::Pull::None);
  usart1TxPin.setAlternate(hal::gpio::Alternate::Alt0);
  usart1TxPin.setMode(hal::gpio::Mode::Alternate);

  nvic.setPriority(hal::nvic::IRQnType::USART1_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::USART1_Irq);

  usart1.setDataWidth(hal::usart::DataWidth::DataWidth8);
  usart1.setParity(hal::usart::Parity::None);
  usart1.setStopBits(hal::usart::StopBits::StopBits1);
  usart1.setOverSampling(hal::usart::OverSampling::OverSampling16);
  usart1.setTransferDirection(hal::usart::TransferDirection::TxRx);
  usart1.setHardwareFlowControl(hal::usart::HardwardFlowControl::None);
  usart1.setPrescaler(hal::usart::Prescaler::DIV_1);
  usart1.setBaudRate(&rcc, 57600);

  usart1.setTXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.setRXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.disableFIFO();
  usart1.configAsyncMode();

  usart1.enable();
}

void setupUSART2() {
  clocks.enableUSART2Clock();
  clocks.enableGPIOAClock();

  usart2RxPin.setSpeed(hal::gpio::Speed::Low);
  usart2RxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart2RxPin.setPull(hal::gpio::Pull::None);
  usart2RxPin.setAlternate(hal::gpio::Alternate::Alt1);
  usart2RxPin.setMode(hal::gpio::Mode::Alternate);

  usart2TxPin.setSpeed(hal::gpio::Speed::Low);
  usart2TxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart2TxPin.setPull(hal::gpio::Pull::None);
  usart2TxPin.setAlternate(hal::gpio::Alternate::Alt1);
  usart2TxPin.setMode(hal::gpio::Mode::Alternate);

  nvic.setPriority(hal::nvic::IRQnType::USART2_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::USART2_Irq);

  usart2.setDataWidth(hal::usart::DataWidth::DataWidth8);
  usart2.setParity(hal::usart::Parity::None);
  usart2.setStopBits(hal::usart::StopBits::StopBits1);
  usart2.setOverSampling(hal::usart::OverSampling::OverSampling16);
  usart2.setTransferDirection(hal::usart::TransferDirection::TxRx);
  usart2.setHardwareFlowControl(hal::usart::HardwardFlowControl::None);
  usart2.setPrescaler(hal::usart::Prescaler::DIV_1);
  usart2.setBaudRate(&rcc, 57600);

  usart2.configAsyncMode();

  usart2.enable();
}

void setupIRTIM() {
  halSystem.setIRModulationEnvelopeSignalSource(hal::system::IRModulationEnvelopeSignalSource::IR_TIM16);
  halSystem.setIRPolarity(hal::system::IRPolarity::NotInverted);
}

void setupTIM3() {
  clocks.enableTIM3Clock();
  clocks.enableGPIOAClock();

  irRxPin.setSpeed(hal::gpio::Speed::Low);
  irRxPin.setOutputType(hal::gpio::OutputType::PushPull);
  irRxPin.setPull(hal::gpio::Pull::None);
  irRxPin.setAlternate(hal::gpio::Alternate::Alt1);
  irRxPin.setMode(hal::gpio::Mode::Alternate);

  // TODO
  irRxDmaChannel.setPeripheralRequest(hal::dma::PeripheralRequest::TIM3_CH1);
  irRxDmaChannel.setDataTransferDirection(hal::dma::TransferDirection::PeripheralToMemory);
  irRxDmaChannel.setChannelPriorityLevel(hal::dma::Priority::Low);
  irRxDmaChannel.setMode(hal::dma::Mode::Circular);
  irRxDmaChannel.setPeripheralIncrementMode(hal::dma::PeripheralIncrementMode::NoIncrement);
  irRxDmaChannel.setMemoryIncrementMode(hal::dma::MemoryIncrementMode::Increment);
  irRxDmaChannel.setPeripheralSize(hal::dma::PeripheralSize::HalfWord);
  irRxDmaChannel.setMemorySize(hal::dma::MemorySize::HalfWord);

  nvic.setPriority(hal::nvic::IRQnType::TIM3_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::TIM3_Irq);

  irRxTimer.setCounterMode(hal::timer::CounterMode::Up);
  irRxTimer.setClockDivision(hal::timer::ClockDivision::DIV_1);
  irRxTimer.setAutoReload(65535);
  irRxTimer.setPrescaler(0);
  irRxTimer.disableAutoReloadPreload();
  irRxTimer.setTriggerOutput(hal::timer::TriggerOutput::Reset);
  irRxTimer.disableMasterSlaveMode();
  irRxTimer.setInputCaptureActiveInput(hal::timer::Channel::Channel1, hal::timer::InputCaptureActiveInput::DirectTI);
  irRxTimer.setInputCapturePrescaler(hal::timer::Channel::Channel1, hal::timer::InputCapturePrescaler::DIV_1);
  irRxTimer.setInputCaptureFilter(hal::timer::Channel::Channel1, hal::timer::InputCaptureFilter::FDIV1);
  irRxTimer.setInputCapturePolarity(hal::timer::Channel::Channel1, hal::timer::InputCapturePolarity::BothEdges);
}