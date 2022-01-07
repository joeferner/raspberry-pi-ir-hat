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
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOFAddress, hal::gpio::GPIOPin::Pin2> resetPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7> irInLedPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9> irOutPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7> usart1RxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6> usart1TxPin;
extern hal::USART<hal::usart::USARTAddress::USART1Address> usart1;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin;
extern hal::USART<hal::usart::USARTAddress::USART2Address> usart2;
extern hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin6> irRxPin;
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
static void setupADC1();

void setup() {
  bus.enableSyscfgClock();
  bus.enablePwrClock();
  setupSystemClock();
  setupGPIO();
  setupDMA();
  setupIWDG();
  setupADC1();

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
  // TODO current_sensor_setup();
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
