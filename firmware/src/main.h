#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cstring>

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

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

const size_t USART_TX_BUFFER_SIZE = 128;
const size_t USART_RX_BUFFER_SIZE = 128;

const size_t IR_TX_BUFFER_SAMPLES = 128;

const size_t IR_RX_BUFFER_SAMPLES = 128;

#ifdef __cplusplus
}
#endif

#endif