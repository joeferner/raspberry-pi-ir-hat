#include "current_sensor.h"
#include "debug.h"
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
#include "ir_rx.h"
#include "ir_tx.h"
#include "main.h"
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
extern hal::Timer irTxCarrierTimer;
extern hal::Timer irTxSignalTimer;
extern hal::IWDGHal iwdg;

static const uint32_t HCLK_FREQUENCY = 16000000;

static void setupSystemClock();
static void setupGPIO();
static void setupDMA();
static void setupUSART1();
static void setupUSART2();
static void setupIRTIM();
static void setupTIM3();
static void setupTIM16();
static void setupTIM17();
static void setupIWDG();
static void setupADC1();

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
  setupTIM16();
  setupTIM17();
  setupIWDG();
  setupADC1();

  time_setup();
  debug_setup();
  rpi_setup();
  ir_rx_setup();
  ir_tx_setup();
  current_sensor_setup();
  debug_send_string("?READY\n");
  LL_IWDG_Enable(IWDG);
}

static void setupSystemClock() {
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

static void setupDMA() {
  clocks.enableDMA1Clock();
  nvic.setPriority(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::DMA1_Ch4_5_DMAMUX1_OVR_Irq);
}

static void setupUSART1() {
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

static void setupUSART2() {
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

static void setupIRTIM() {
  halSystem.setIRModulationEnvelopeSignalSource(hal::system::IRModulationEnvelopeSignalSource::IR_TIM16);
  halSystem.setIRPolarity(hal::system::IRPolarity::NotInverted);
}

static void setupTIM3() {
  clocks.enableTIM3Clock();
  clocks.enableGPIOAClock();

  irRxPin.setSpeed(hal::gpio::Speed::Low);
  irRxPin.setOutputType(hal::gpio::OutputType::PushPull);
  irRxPin.setPull(hal::gpio::Pull::None);
  irRxPin.setAlternate(hal::gpio::Alternate::Alt1);
  irRxPin.setMode(hal::gpio::Mode::Alternate);

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
  irRxTimer.setRepetitionCounter(0);
  irRxTimer.disableAutoReloadPreload();
  irRxTimer.setTriggerOutput(hal::timer::TriggerOutput::Reset);
  irRxTimer.disableMasterSlaveMode();
  irRxTimer.setInputCaptureActiveInput(hal::timer::Channel::Channel1, hal::timer::InputCaptureActiveInput::DirectTI);
  irRxTimer.setInputCapturePrescaler(hal::timer::Channel::Channel1, hal::timer::InputCapturePrescaler::DIV_1);
  irRxTimer.setInputCaptureFilter(hal::timer::Channel::Channel1, hal::timer::InputCaptureFilter::FDIV1);
  irRxTimer.setInputCapturePolarity(hal::timer::Channel::Channel1, hal::timer::InputCapturePolarity::BothEdges);
}

static void setupTIM16() {
  clocks.enableTIM16Clock();

  nvic.setPriority(hal::nvic::IRQnType::TIM16_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::TIM16_Irq);

  irTxSignalTimer.setCounterMode(hal::timer::CounterMode::Up);
  irTxSignalTimer.setClockDivision(hal::timer::ClockDivision::DIV_1);
  irTxSignalTimer.setAutoReload(65535);
  irTxSignalTimer.setPrescaler(0);
  irTxSignalTimer.setRepetitionCounter(0);
  irTxSignalTimer.disableAutoReloadPreload();
  irTxSignalTimer.enableOutputComparePreload(hal::timer::Channel::Channel1);
  irTxSignalTimer.disableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
  irTxSignalTimer.disableCaptureCompareChannel(hal::timer::ChannelN::Channel1N);
  irTxSignalTimer.setOutputCompareMode(hal::timer::Channel::Channel1, hal::timer::OutputCompareMode::PWM1);
  irTxSignalTimer.setOutputComparePolarity(hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
  irTxSignalTimer.setOutputComparePolarity(hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
  // TODO set this to high?
  irTxSignalTimer.setOutputCompareIdleState(hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
  irTxSignalTimer.setOutputCompareIdleState(hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
  irTxSignalTimer.setOutputCompareValue(hal::timer::Channel::Channel1, 0);
  irTxSignalTimer.disableOutputCompareFast(hal::timer::Channel::Channel1);
  // TODO
  // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  // TIM_BDTRInitStruct.DeadTime = 0;
  // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
  // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  // LL_TIM_BDTR_Init(TIM16, &TIM_BDTRInitStruct);
}

static void setupTIM17() {
  clocks.enableTIM17Clock();

  irTxCarrierTimer.setCounterMode(hal::timer::CounterMode::Up);
  irTxCarrierTimer.setClockDivision(hal::timer::ClockDivision::DIV_1);
  irTxCarrierTimer.setAutoReload(65535);
  irTxCarrierTimer.setPrescaler(0);
  irTxCarrierTimer.setRepetitionCounter(0);
  irTxCarrierTimer.disableAutoReloadPreload();
  irTxCarrierTimer.enableOutputComparePreload(hal::timer::Channel::Channel1);
  irTxCarrierTimer.disableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
  irTxCarrierTimer.disableCaptureCompareChannel(hal::timer::ChannelN::Channel1N);
  irTxCarrierTimer.setOutputCompareMode(hal::timer::Channel::Channel1, hal::timer::OutputCompareMode::PWM1);
  irTxCarrierTimer.setOutputComparePolarity(hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
  irTxCarrierTimer.setOutputComparePolarity(hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
  // TODO set this to high?
  irTxCarrierTimer.setOutputCompareIdleState(hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
  irTxCarrierTimer.setOutputCompareIdleState(hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
  irTxCarrierTimer.setOutputCompareValue(hal::timer::Channel::Channel1, 0);
  irTxCarrierTimer.disableOutputCompareFast(hal::timer::Channel::Channel1);
  // TODO
  // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  // TIM_BDTRInitStruct.DeadTime = 0;
  // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
  // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  // LL_TIM_BDTR_Init(TIM17, &TIM_BDTRInitStruct);
}

static void setupIWDG() {
  iwdg.setPrescaler(hal::iwdg::Prescaler::Prescaler4);
  iwdg.setReloadCounter(4096);
  while (!iwdg.isReady()) {
  }
  iwdg.reloadCounter();
}

static void setupADC1() {
  // TODO
  //   /* Peripheral clock enable */
  //   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

  //   LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  //   /**ADC1 GPIO Configuration
  //   PA0   ------> ADC1_IN0
  //   PA4   ------> ADC1_IN4
  //   PA5   ------> ADC1_IN5
  //   */
  //   GPIO_InitStruct.Pin = CURREF_Pin;
  //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  //   LL_GPIO_Init(CURREF_GPIO_Port, &GPIO_InitStruct);

  //   GPIO_InitStruct.Pin = CUR1_Pin;
  //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  //   LL_GPIO_Init(CUR1_GPIO_Port, &GPIO_InitStruct);

  //   GPIO_InitStruct.Pin = CUR2_Pin;
  //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  //   LL_GPIO_Init(CUR2_GPIO_Port, &GPIO_InitStruct);

  //   /* ADC1 interrupt Init */
  //   NVIC_SetPriority(ADC1_IRQn, 0);
  //   NVIC_EnableIRQ(ADC1_IRQn);

  //   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  //   */

  // #define ADC_CHANNEL_CONF_RDY_TIMEOUT_MS ( 1U)
  // #if (USE_TIMEOUT == 1)
  //   uint32_t Timeout; /* Variable used for Timeout management */
  // #endif /* USE_TIMEOUT */

  //   ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  //   ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  //   ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  //   ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  //   LL_ADC_Init(ADC1, &ADC_InitStruct);
  //   LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_CONFIGURABLE);

  //   /* Poll for ADC channel configuration ready */
  // #if (USE_TIMEOUT == 1)
  //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
  // #endif /* USE_TIMEOUT */
  //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
  //   {
  // #if (USE_TIMEOUT == 1)
  //     /* Check Systick counter flag to decrement the time-out value */
  //     if (LL_SYSTICK_IsActiveCounterFlag())
  //     {
  //       if (Timeout-- == 0)
  //       {
  //         Error_Handler();
  //       }
  //     }
  // #endif /* USE_TIMEOUT */
  //   }
  //   /* Clear flag ADC channel configuration ready */
  //   LL_ADC_ClearFlag_CCRDY(ADC1);
  //   ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  //   ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
  //   ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_1RANK;
  //   ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  //   ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  //   ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  //   LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  //   LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
  //   LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
  //   LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_12CYCLES_5);
  //   LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_2, LL_ADC_SAMPLINGTIME_12CYCLES_5);
  //   LL_ADC_DisableIT_EOC(ADC1);
  //   LL_ADC_DisableIT_EOS(ADC1);

  //   /* Enable ADC internal voltage regulator */
  //   LL_ADC_EnableInternalRegulator(ADC1);
  //   /* Delay for ADC internal voltage regulator stabilization. */
  //   /* Compute number of CPU cycles to wait for, from delay in us. */
  //   /* Note: Variable divided by 2 to compensate partially */
  //   /* CPU processing cycles (depends on compilation optimization). */
  //   /* Note: If system core clock frequency is below 200kHz, wait time */
  //   /* is only a few CPU processing cycles. */
  //   uint32_t wait_loop_index;
  //   wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
  //   while (wait_loop_index != 0)
  //   {
  //     wait_loop_index--;
  //   }
  //   /** Configure Regular Channel
  //   */
  //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
  //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_COMMON_1);
  //   /** Configure Regular Channel
  //   */
  //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_4);

  //   /* Poll for ADC channel configuration ready */
  // #if (USE_TIMEOUT == 1)
  //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
  // #endif /* USE_TIMEOUT */
  //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
  //   {
  // #if (USE_TIMEOUT == 1)
  //     /* Check Systick counter flag to decrement the time-out value */
  //     if (LL_SYSTICK_IsActiveCounterFlag())
  //     {
  //       if (Timeout-- == 0)
  //       {
  //         Error_Handler();
  //       }
  //     }
  // #endif /* USE_TIMEOUT */
  //   }
  //   /* Clear flag ADC channel configuration ready */
  //   LL_ADC_ClearFlag_CCRDY(ADC1);
  //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_COMMON_1);
  //   /** Configure Regular Channel
  //   */
  //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_5);

  //   /* Poll for ADC channel configuration ready */
  // #if (USE_TIMEOUT == 1)
  //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
  // #endif /* USE_TIMEOUT */
  //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
  //   {
  // #if (USE_TIMEOUT == 1)
  //     /* Check Systick counter flag to decrement the time-out value */
  //     if (LL_SYSTICK_IsActiveCounterFlag())
  //     {
  //       if (Timeout-- == 0)
  //       {
  //         Error_Handler();
  //       }
  //     }
  // #endif /* USE_TIMEOUT */
  //   }
  //   /* Clear flag ADC channel configuration ready */
  //   LL_ADC_ClearFlag_CCRDY(ADC1);
  //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_COMMON_1);
}