#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g031xx.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_iwdg.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_utils.h"
#include "hal/Clocks.hpp"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

void Error_Handler(void);

// TODO begin remove
#define DISABLE_IRQS()                \
  uint32_t primask = __get_PRIMASK(); \
  __set_PRIMASK(1);

#define ENABLE_IRQS() __set_PRIMASK(primask);

#define CONCAT_(A, B) A##B
#define CONCAT(A, B) CONCAT_(A, B)

#define DEBUG_USART USART1
#define DEBUG_TX_BUFFER_LEN 500
#define DEBUG_RX_BUFFER_LEN 20

#define RPI_USART USART2
#define RPI_TX_BUFFER_LEN 200
#define RPI_RX_BUFFER_LEN 200

#define IR_RX_TIMER TIM3
#define IR_RX_TIMER_CH_NO 1
#define IR_RX_TIMER_CH CONCAT(LL_TIM_CHANNEL_CH, IR_RX_TIMER_CH_NO)
#define IR_RX_DMA DMA1
#define IR_RX_DMA_CH_NO 5
#define IR_RX_DMA_CH CONCAT(LL_DMA_CHANNEL_, IR_RX_DMA_CH_NO)
#define IR_RX_BUFFER_SAMPLES 128
#define IR_RX_CAPTURE_REG_ADDR (&(IR_RX_TIMER->CCR1))
#define IR_RX_LL_TIM_EnableIT_CC() CONCAT(LL_TIM_EnableIT_CC, IR_RX_TIMER_CH_NO)(IR_RX_TIMER)
#define IR_RX_LL_TIM_EnableDMAReq_CC() CONCAT(LL_TIM_EnableDMAReq_CC, IR_RX_TIMER_CH_NO)(IR_RX_TIMER)
#define IR_RX_LL_DMA_ClearFlag_RX_GI() CONCAT(LL_DMA_ClearFlag_GI, IR_RX_DMA_CH_NO)(IR_RX_DMA)
#define IR_RX_LL_DMA_IsActiveFlag_RX_TC() CONCAT(LL_DMA_IsActiveFlag_TC, IR_RX_DMA_CH_NO)(IR_RX_DMA)
#define IR_RX_LL_DMA_IsActiveFlag_RX_GI() CONCAT(LL_DMA_IsActiveFlag_GI, IR_RX_DMA_CH_NO)(IR_RX_DMA)

#define IR_OUT_CARRIER_TIMER TIM17
#define IR_OUT_CARRIER_CHANNEL LL_TIM_CHANNEL_CH1
#define IR_OUT_CARRIER_TIM_OC_SetCompare(on) LL_TIM_OC_SetCompareCH1(IR_OUT_CARRIER_TIMER, on)
#define IR_OUT_CARRIER_PRESCALER 0
#define IR_OUT_SIGNAL_TIMER TIM16
#define IR_OUT_SIGNAL_CHANNEL LL_TIM_CHANNEL_CH1
#define IR_OUT_SIGNAL_TIM_OC_SetCompare(on) LL_TIM_OC_SetCompareCH1(IR_OUT_SIGNAL_TIMER, on)
#define IR_OUT_SIGNAL_PRESCALER 10
// TODO end remove

#ifdef __cplusplus
}
#endif

#endif