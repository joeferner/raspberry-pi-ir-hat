#include <stm32f0xx_hal.h>

extern void main_error_handler();

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

#define COM_UART    huart1
#define IR_TX_TIMER htim1
#define IR_RX_TIMER htim2
#define IR_RX_CH    TIM_CHANNEL_1
#define IR_RX_BUFFER_SIZE 100