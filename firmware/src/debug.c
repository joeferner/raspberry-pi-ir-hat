#include <stdlib.h>
#include <stm32f0xx_ll_usart.h>
#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_ll_bus.h>
#include <stm32f0xx_ll_dma.h>
#include "config.h"
#include "debug.h"
#include "lwrb/lwrb.h"

static lwrb_t uart_tx_dma_ringbuff;
static uint8_t uart_tx_dma_lwrb_data[256];
static volatile size_t uart_tx_dma_current_len;

static lwrb_t uart_rx_dma_ringbuff;
static uint8_t uart_rx_dma_lwrb_data[256];
static int uart_rx_old_pos = 0;

void uart_start_tx_dma_transfer();

void debug_dma_tx_complete();

void debug_dma_rx_complete();

#define uart_rx_get_pos() (sizeof(uart_rx_dma_lwrb_data) - LL_DMA_GetDataLength(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH))

void debug_setup() {
    // Peripheral clock enable
    LL_APB1_GRP2_EnableClock(DEBUG_USART_CLOCK);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    // USART GPIO Configuration
    // PA9   ------> USART_TX
    // PA10   ------> USART_RX
    LL_GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = LL_GPIO_PIN_9;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &gpio_init);

    gpio_init.Pin = LL_GPIO_PIN_10;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &gpio_init);

    // USART DMA TX Init
    LL_DMA_SetDataTransferDirection(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_EnableIT_TC(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH);
    LL_DMA_EnableIT_TE(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH);

    // USART DMA RX Init
    LL_DMA_SetDataTransferDirection(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_EnableIT_TC(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH);
    LL_DMA_EnableIT_TE(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH);

    // USART init
    LL_USART_InitTypeDef usart_init = {0};
    usart_init.BaudRate = DEBUG_BAUD_RATE;
    usart_init.DataWidth = LL_USART_DATAWIDTH_8B;
    usart_init.StopBits = LL_USART_STOPBITS_1;
    usart_init.Parity = LL_USART_PARITY_NONE;
    usart_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usart_init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart_init.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(DEBUG_USART, &usart_init);
    LL_USART_DisableIT_CTS(DEBUG_USART);
    LL_USART_ConfigAsyncMode(DEBUG_USART);
    LL_USART_EnableIT_PE(DEBUG_USART); // enable parity errors
    LL_USART_EnableIT_ERROR(DEBUG_USART);
    LL_USART_Enable(DEBUG_USART);

    // ring buffers init
    lwrb_init(&uart_tx_dma_ringbuff, uart_tx_dma_lwrb_data, sizeof(uart_tx_dma_lwrb_data));
    lwrb_init(&uart_rx_dma_ringbuff, uart_rx_dma_lwrb_data, sizeof(uart_rx_dma_lwrb_data));
    uart_tx_dma_current_len = 0;

    // begin rx dma
    LL_DMA_DisableChannel(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH);
    DEBUG_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, sizeof(uart_rx_dma_lwrb_data));
    LL_DMA_SetPeriphAddress(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, DEBUG_USART->RDR);
    LL_DMA_SetMemoryAddress(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH, (uint32_t) uart_rx_dma_lwrb_data);
    LL_DMA_EnableChannel(DEBUG_TX_RX_DMA, DEBUG_RX_DMA_CH);
    LL_USART_EnableDMAReq_RX(DEBUG_USART);

    uart_rx_old_pos = uart_rx_get_pos();
}

void debug_loop() {
    debug_dma_rx_complete();

    uint8_t peek_buffer[256];
    size_t peek_size;
    peek_size = lwrb_peek(&uart_rx_dma_ringbuff, 0, peek_buffer, sizeof(peek_buffer));
    for (size_t i = 0; i < peek_size; i++) {
        if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
            i++;
            peek_buffer[i] = '\0';
            lwrb_skip(&uart_rx_dma_ringbuff, i);
            debug_rx(peek_buffer, i);
            break;
        }
    }
}

void debug_send_string(const char *str) {
    debug_tx((const uint8_t *) str, strlen(str));
}

void debug_send_uint32(uint32_t value) {
    char buffer[15];
    utoa(value, buffer, 10);
    debug_send_string(buffer);
}

void debug_tx(const uint8_t *data, size_t data_len) {
    if (data_len > sizeof(uart_tx_dma_lwrb_data)) {
        main_error_handler();
    }
    while (lwrb_get_free(&uart_tx_dma_ringbuff) < data_len);
    lwrb_write(&uart_tx_dma_ringbuff, data, data_len);
    uart_start_tx_dma_transfer();
}

void uart_start_tx_dma_transfer() {
    if (uart_tx_dma_current_len == 0) {
        uart_tx_dma_current_len = lwrb_get_linear_block_read_length(&uart_tx_dma_ringbuff);
        if (uart_tx_dma_current_len > 0) {
            void *p = lwrb_get_linear_block_read_address(&uart_tx_dma_ringbuff);

            LL_DMA_DisableChannel(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH);
            DEBUG_LL_DMA_ClearFlag_TX_GI();
            LL_DMA_SetDataLength(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, uart_tx_dma_current_len);
            LL_DMA_SetPeriphAddress(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, DEBUG_USART->TDR);
            LL_DMA_SetMemoryAddress(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH, (uint32_t) p);
            LL_DMA_EnableChannel(DEBUG_TX_RX_DMA, DEBUG_TX_DMA_CH);
            LL_USART_ClearFlag_TC(DEBUG_USART);
            LL_USART_EnableDMAReq_TX(DEBUG_USART);
        }
    }
}

void debug_dma_irq() {
    if (DEBUG_LL_DMA_IsActiveFlag_TX_TC()) {
        DEBUG_LL_DMA_ClearFlag_TX_GI();
        debug_dma_tx_complete();
    }

    if (DEBUG_LL_DMA_IsActiveFlag_RX_TC()) {
        DEBUG_LL_DMA_ClearFlag_RX_GI();
        debug_dma_rx_complete();
    }
}

void debug_dma_tx_complete() {
    lwrb_skip(&uart_tx_dma_ringbuff, uart_tx_dma_current_len);
    uart_tx_dma_current_len = 0;
    uart_start_tx_dma_transfer();
}

void debug_dma_rx_complete() {
    size_t pos = uart_rx_get_pos();
    if (pos != uart_rx_old_pos) {
        if (pos > uart_rx_old_pos) {
            // not wrapped
            lwrb_advance(&uart_rx_dma_ringbuff, pos - uart_rx_old_pos);
        } else {
            // wrapped
            lwrb_advance(&uart_rx_dma_ringbuff, (sizeof(uart_rx_dma_lwrb_data) - uart_rx_old_pos) + pos);
        }
    }
    uart_rx_old_pos = pos;
    if (uart_rx_old_pos == sizeof(uart_rx_dma_lwrb_data)) {
        uart_rx_old_pos = 0;
    }
}
