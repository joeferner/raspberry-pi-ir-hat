extern void main_error_handler();

#define DEBUG_BAUD_RATE   57600
#define DEBUG_USART       USART1
#define DEBUG_USART_CLOCK LL_APB1_GRP2_PERIPH_USART1
#define DEBUG_TX_RX_DMA   DMA1
#define DEBUG_TX_DMA_CH   LL_DMA_CHANNEL_2
#define DEBUG_RX_DMA_CH   LL_DMA_CHANNEL_3
#define DEBUG_LL_DMA_ClearFlag_TX_GI()     LL_DMA_ClearFlag_GI2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_ClearFlag_RX_GI()     LL_DMA_ClearFlag_GI3(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_TX_TC()  LL_DMA_IsActiveFlag_TC2(DEBUG_TX_RX_DMA)
#define DEBUG_LL_DMA_IsActiveFlag_RX_TC()  LL_DMA_IsActiveFlag_TC3(DEBUG_TX_RX_DMA)

#define IR_RX_TIMER             htim2
#define IR_RX_CH                TIM_CHANNEL_1
#define IR_RX_BUFFER_SAMPLES    100
#define IR_TX_TIMER_CARRIER     htim17
#define IR_TX_TIMER_CARRIER_CH  TIM_CHANNEL_1
#define IR_TX_TIMER_SIGNAL      htim16
#define IR_TX_TIMER_SIGNAL_CH   TIM_CHANNEL_1
