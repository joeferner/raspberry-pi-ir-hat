#include <stm32g0xx_ll_system.h>
#include <stm32g0xx_ll_rcc.h>
#include <stm32g0xx_ll_utils.h>
#include <stm32g0xx_ll_bus.h>
#include <stm32g0xx_ll_gpio.h>
#include <stm32g0xx_ll_tim.h>
#include <stm32g0xx_ll_dma.h>
#include <stm32g0xx_ll_usart.h>
#include <stm32g0xx_ll_iwdg.h>
#include "config_generated.h"

extern void Error_Handler();

#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A,B)

#define DEBUG_USART        USART1
#define DEBUG_TX_DMA       DMA1
#define DEBUG_TX_DMA_CH_NO 1
#define DEBUG_TX_DMA_CH    CONCAT(LL_DMA_CHANNEL_, DEBUG_TX_DMA_CH_NO)
#define DEBUG_RX_DMA       DMA1
#define DEBUG_RX_DMA_CH_NO 2
#define DEBUG_RX_DMA_CH    CONCAT(LL_DMA_CHANNEL_, DEBUG_RX_DMA_CH_NO)
#define DEBUG_LL_DMA_ClearFlag_TX_GI()     CONCAT(LL_DMA_ClearFlag_GI, DEBUG_TX_DMA_CH_NO)(DEBUG_TX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_TC()  CONCAT(LL_DMA_IsActiveFlag_TC, DEBUG_TX_DMA_CH_NO)(DEBUG_TX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_GI()  CONCAT(LL_DMA_IsActiveFlag_GI, DEBUG_TX_DMA_CH_NO)(DEBUG_TX_DMA)
#define DEBUG_LL_DMA_ClearFlag_RX_GI()     CONCAT(LL_DMA_ClearFlag_GI, DEBUG_RX_DMA_CH_NO)(DEBUG_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_TC()  CONCAT(LL_DMA_IsActiveFlag_TC, DEBUG_RX_DMA_CH_NO)(DEBUG_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_GI()  CONCAT(LL_DMA_IsActiveFlag_GI, DEBUG_RX_DMA_CH_NO)(DEBUG_RX_DMA)

#define RPI_USART        USART2
#define RPI_TX_DMA       DMA1
#define RPI_TX_DMA_CH_NO 4
#define RPI_TX_DMA_CH    CONCAT(LL_DMA_CHANNEL_, RPI_TX_DMA_CH_NO)
#define RPI_RX_DMA       DMA1
#define RPI_RX_DMA_CH_NO 3
#define RPI_RX_DMA_CH    CONCAT(LL_DMA_CHANNEL_, RPI_RX_DMA_CH_NO)
#define RPI_LL_DMA_ClearFlag_TX_GI()     CONCAT(LL_DMA_ClearFlag_GI, RPI_TX_DMA_CH_NO)(RPI_TX_DMA)
#define RPI_LL_DMA_IsActiveFlag_TX_TC()  CONCAT(LL_DMA_IsActiveFlag_TC, RPI_TX_DMA_CH_NO)(RPI_TX_DMA)
#define RPI_LL_DMA_IsActiveFlag_TX_GI()  CONCAT(LL_DMA_IsActiveFlag_GI, RPI_TX_DMA_CH_NO)(RPI_TX_DMA)
#define RPI_LL_DMA_ClearFlag_RX_GI()     CONCAT(LL_DMA_ClearFlag_GI, RPI_RX_DMA_CH_NO)(RPI_RX_DMA)
#define RPI_LL_DMA_IsActiveFlag_RX_TC()  CONCAT(LL_DMA_IsActiveFlag_TC, RPI_RX_DMA_CH_NO)(RPI_RX_DMA)
#define RPI_LL_DMA_IsActiveFlag_RX_GI()  CONCAT(LL_DMA_IsActiveFlag_GI, RPI_RX_DMA_CH_NO)(RPI_RX_DMA)

#define IR_RX_TIMER           TIM3
#define IR_RX_TIMER_CH_NO     1
#define IR_RX_TIMER_CH        CONCAT(LL_TIM_CHANNEL_CH, IR_RX_TIMER_CH_NO)
#define IR_RX_DMA             DMA1
#define IR_RX_DMA_CH_NO       5
#define IR_RX_DMA_CH          CONCAT(LL_DMA_CHANNEL_, IR_RX_DMA_CH_NO)
#define IR_RX_BUFFER_SAMPLES  128
#define IR_RX_CAPTURE_REG_ADDR            (&(IR_RX_TIMER->CCR1))
#define IR_RX_LL_TIM_EnableIT_CC()        CONCAT(LL_TIM_EnableIT_CC, IR_RX_TIMER_CH_NO)(IR_RX_TIMER)
#define IR_RX_LL_TIM_EnableDMAReq_CC()    CONCAT(LL_TIM_EnableDMAReq_CC, IR_RX_TIMER_CH_NO)(IR_RX_TIMER)
#define IR_RX_LL_DMA_ClearFlag_RX_GI()    CONCAT(LL_DMA_ClearFlag_GI, IR_RX_DMA_CH_NO)(IR_RX_DMA)
#define IR_RX_LL_DMA_IsActiveFlag_RX_TC() CONCAT(LL_DMA_IsActiveFlag_TC, IR_RX_DMA_CH_NO)(IR_RX_DMA)
#define IR_RX_LL_DMA_IsActiveFlag_RX_GI() CONCAT(LL_DMA_IsActiveFlag_GI, IR_RX_DMA_CH_NO)(IR_RX_DMA)

#define IR_OUT_CARRIER_TIMER            TIM17
#define IR_OUT_CARRIER_CHANNEL          LL_TIM_CHANNEL_CH1
#define IR_OUT_CARRIER_TIM_OC_SetCompare(on) LL_TIM_OC_SetCompareCH1(IR_OUT_CARRIER_TIMER, on)
#define IR_OUT_CARRIER_PRESCALER        0
#define IR_OUT_SIGNAL_TIMER             TIM16
#define IR_OUT_SIGNAL_CHANNEL           LL_TIM_CHANNEL_CH1
#define IR_OUT_SIGNAL_TIM_OC_SetCompare(on) LL_TIM_OC_SetCompareCH1(IR_OUT_SIGNAL_TIMER, on)
#define IR_OUT_SIGNAL_PRESCALER         10
