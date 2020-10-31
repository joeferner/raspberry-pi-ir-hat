extern void Error_Handler();

#define DEBUG_BAUD_RATE   57600
#define DEBUG_USART       USART1
#define DEBUG_USART_CLOCK LL_APB1_GRP2_PERIPH_USART1
#define DEBUG_TX_PORT     GPIOA
#define DEBUG_TX_PIN      LL_GPIO_PIN_9
#define DEBUG_RX_PORT     GPIOA
#define DEBUG_RX_PIN      LL_GPIO_PIN_10
#define DEBUG_TX_RX_DMA   DMA1
#define DEBUG_TX_DMA_CH   LL_DMA_CHANNEL_2
#define DEBUG_RX_DMA_CH   LL_DMA_CHANNEL_3
#define DEBUG_LL_DMA_ClearFlag_TX_GI()     LL_DMA_ClearFlag_GI2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_ClearFlag_RX_GI()     LL_DMA_ClearFlag_GI3(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_TC()  LL_DMA_IsActiveFlag_TC2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_GI()  LL_DMA_IsActiveFlag_GI2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_TC()  LL_DMA_IsActiveFlag_TC3(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_GI()  LL_DMA_IsActiveFlag_GI3(DEBUG_TX_RX_DMA)

#define IR_RX_PERIPH_TIMER    LL_APB1_GRP1_PERIPH_TIM2
#define IR_RX_TIMER           TIM2
#define IR_RX_TIMER_CH        LL_TIM_CHANNEL_CH1
#define IR_RX_TIMER_IRQ       TIM2_IRQn
#define IR_RX_DMA             DMA1
#define IR_RX_DMA_CH          LL_DMA_CHANNEL_5
#define IR_RX_PORT            GPIOA
#define IR_RX_PIN             LL_GPIO_PIN_0
// SystemCoreClock (8000000)
#define IR_RX_TIMER_PRESCALER 7
#define IR_RX_BUFFER_SAMPLES  100
#define IR_RX_CAPTURE_REG_ADDR          (&(IR_RX_TIMER->CCR1))
#define IR_RX_LL_TIM_EnableIT_CC()      LL_TIM_EnableIT_CC1(IR_RX_TIMER)
#define IR_RX_LL_TIM_EnableDMAReq_CC()  LL_TIM_EnableDMAReq_CC1(IR_RX_TIMER)
#define IR_RX_LL_DMA_ClearFlag_RX_GI()  LL_DMA_ClearFlag_GI5(IR_RX_DMA)
#define IR_RX_LL_TIM_IsActiveFlag_CC()  LL_TIM_IsActiveFlag_CC1(IR_RX_TIMER)

#define IR_RX_CH                TIM_CHANNEL_1

#define IR_TX_TIMER_CARRIER     htim17
#define IR_TX_TIMER_CARRIER_CH  TIM_CHANNEL_1
#define IR_TX_TIMER_SIGNAL      htim16
#define IR_TX_TIMER_SIGNAL_CH   TIM_CHANNEL_1
