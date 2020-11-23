#include <stm32g0xx_ll_system.h>
#include <stm32g0xx_ll_rcc.h>
#include <stm32g0xx_ll_utils.h>
#include <stm32g0xx_ll_bus.h>
#include <stm32g0xx_ll_gpio.h>
#include <stm32g0xx_ll_tim.h>
#include <stm32g0xx_ll_dma.h>
#include <stm32g0xx_ll_usart.h>

#define DEBUG_USART       USART1
#define DEBUG_TX_RX_DMA   DMA1
#define DEBUG_TX_DMA_CH   LL_DMA_CHANNEL_1
#define DEBUG_RX_DMA_CH   LL_DMA_CHANNEL_2
#define DEBUG_LL_DMA_ClearFlag_TX_GI()     LL_DMA_ClearFlag_GI2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_ClearFlag_RX_GI()     LL_DMA_ClearFlag_GI3(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_TC()  LL_DMA_IsActiveFlag_TC2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_GI()  LL_DMA_IsActiveFlag_GI2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_TC()  LL_DMA_IsActiveFlag_TC3(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_GI()  LL_DMA_IsActiveFlag_GI3(DEBUG_TX_RX_DMA)

#define IR_RX_TIMER           TIM3
#define IR_RX_TIMER_CH        LL_TIM_CHANNEL_CH1
#define IR_RX_TIMER_IRQ       TIM3_IRQn
#define IR_RX_DMA             DMA1
#define IR_RX_DMA_CH          LL_DMA_CHANNEL_5
#define IR_RX_TIMER_PRESCALER 7
#define IR_RX_BUFFER_SAMPLES  100
#define IR_RX_CAPTURE_REG_ADDR          (&(IR_RX_TIMER->CCR1))
#define IR_RX_LL_TIM_EnableIT_CC()      LL_TIM_EnableIT_CC1(IR_RX_TIMER)
#define IR_RX_LL_TIM_EnableDMAReq_CC()  LL_TIM_EnableDMAReq_CC1(IR_RX_TIMER)
#define IR_RX_LL_DMA_ClearFlag_RX_GI()  LL_DMA_ClearFlag_GI5(IR_RX_DMA)
#define IR_RX_LL_TIM_IsActiveFlag_CC()  LL_TIM_IsActiveFlag_CC1(IR_RX_TIMER)

#define IR_TX_PORT                     GPIOB
#define IR_TX_PIN                      LL_GPIO_PIN_9
#define IR_TX_AF                       LL_GPIO_AF_0
#define IR_TX_CARRIER_TIMER            TIM17
#define IR_TX_CARRIER_CHANNEL          LL_TIM_CHANNEL_CH1
#define IR_TX_CARRIER_TIM_OC_SetCompareCH1(on) LL_TIM_OC_SetCompareCH1(IR_TX_CARRIER_TIMER, on)
#define IR_TX_SIGNAL_TIMER             TIM16
#define IR_TX_SIGNAL_CHANNEL           LL_TIM_CHANNEL_CH1
#define IR_TX_SIGNAL_IRQ               TIM16_IRQn
#define IR_TX_LL_TIM_OC_SetCompare(on) LL_TIM_OC_SetCompareCH1(IR_TX_SIGNAL_TIMER, on)
