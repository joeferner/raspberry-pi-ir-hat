#include "config.h"
#include "rpi.h"
#include "dma_tx_ring_buffer.h"
#include "dma_rx_ring_buffer.h"

#define RX_BUFFER_LEN 512

static dma_tx_ring_buffer uart_tx_dma_ring_buffer;
static uint8_t uart_tx_dma_ring_buffer_data[256] __attribute__((aligned(8)));

static dma_rx_ring_buffer uart_rx_dma_ring_buffer;
static uint8_t uart_rx_dma_ring_buffer_data[RX_BUFFER_LEN] __attribute__((aligned(8)));

static void rpi_dma_tx_complete();

static void rpi_tx_begin_dma(struct dma_tx_ring_buffer_t *rb);

void rpi_setup()
{
    // USART DMA TX Init
    LL_DMA_EnableIT_TC(RPI_TX_DMA, RPI_TX_DMA_CH);
    LL_DMA_EnableIT_TE(RPI_TX_DMA, RPI_TX_DMA_CH);

    // ring buffers init
    dma_tx_ring_buffer_init(
        &uart_tx_dma_ring_buffer,
        uart_tx_dma_ring_buffer_data,
        sizeof(uart_tx_dma_ring_buffer_data),
        RPI_TX_DMA,
        RPI_TX_DMA_CH,
        rpi_tx_begin_dma);
    dma_rx_ring_buffer_init(
        &uart_rx_dma_ring_buffer,
        uart_rx_dma_ring_buffer_data,
        sizeof(uart_rx_dma_ring_buffer_data),
        RPI_RX_DMA,
        RPI_RX_DMA_CH);

    // begin rx dma
    LL_DMA_DisableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    RPI_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(RPI_RX_DMA, RPI_RX_DMA_CH, sizeof(uart_rx_dma_ring_buffer_data));
    LL_DMA_SetPeriphAddress(RPI_RX_DMA, RPI_RX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(RPI_RX_DMA, RPI_RX_DMA_CH, (uint32_t)uart_rx_dma_ring_buffer_data);
    LL_DMA_EnableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    LL_USART_EnableDMAReq_RX(RPI_USART);
}

void rpi_loop()
{
    uint8_t b;
    while (dma_rx_ring_buffer_peek1(&uart_rx_dma_ring_buffer, &b))
    {
        dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, 1);
    }
}

void rpi_ir_rx_received(uint32_t value)
{
    // TODO send value
}

void rpi_dma_irq()
{
    if (RPI_LL_DMA_IsActiveFlag_TX_TC())
    {
        RPI_LL_DMA_ClearFlag_TX_GI();
        rpi_dma_tx_complete();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_TC())
    {
        RPI_LL_DMA_ClearFlag_RX_GI();
    }

    if (RPI_LL_DMA_IsActiveFlag_TX_GI())
    {
        RPI_LL_DMA_ClearFlag_TX_GI();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_GI())
    {
        RPI_LL_DMA_ClearFlag_RX_GI();
    }
}

void rpi_dma_tx_complete()
{
    dma_tx_ring_buffer_irq(&uart_tx_dma_ring_buffer);
}

static void rpi_tx_begin_dma(struct dma_tx_ring_buffer_t *rb)
{
    LL_DMA_SetPeriphAddress(RPI_TX_DMA, RPI_TX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_TRANSMIT));
    RPI_LL_DMA_ClearFlag_TX_GI();
    LL_USART_ClearFlag_TC(RPI_USART);
    LL_USART_EnableDMAReq_TX(RPI_USART);
}
