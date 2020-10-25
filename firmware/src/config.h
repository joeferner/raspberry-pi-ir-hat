#include <stm32f0xx_hal.h>

extern void main_error_handler();

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

extern UART_HandleTypeDef huart1;