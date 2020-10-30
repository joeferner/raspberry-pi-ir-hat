#include <stm32f0xx_ll_bus.h>
#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_ll_tim.h>
#include <stm32f0xx_ll_dma.h>
#include "config.h"
#include "ir_rx.h"
#include "lwrb/lwrb.h"
#include "time.h"

static ir_rx_value ir_rx_dma_data[IR_RX_BUFFER_SAMPLES];
static size_t ir_rx_dma_data_read_index = 0;

static size_t _ir_rx_get_pos();

void ir_rx_setup() {
    LL_APB1_GRP1_EnableClock(IR_RX_PERIPH_TIMER);

    // TIM2 GPIO Configuration
    // PA0   ------> TIM2_CH1
    LL_GPIO_InitTypeDef gpi_init = {0};
    gpi_init.Pin = IR_RX_PIN;
    gpi_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpi_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpi_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpi_init.Pull = LL_GPIO_PULL_NO;
    gpi_init.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(IR_RX_PORT, &gpi_init);

    LL_TIM_InitTypeDef tim_init = {0};
    tim_init.Prescaler = IR_RX_TIMER_PRESCALER;
    tim_init.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim_init.Autoreload = 4294967295;
    tim_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(IR_RX_TIMER, &tim_init);
    LL_TIM_DisableARRPreload(IR_RX_TIMER);
    LL_TIM_SetTriggerOutput(IR_RX_TIMER, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(IR_RX_TIMER);
    LL_TIM_IC_SetActiveInput(IR_RX_TIMER, IR_RX_TIMER_CH, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(IR_RX_TIMER, IR_RX_TIMER_CH, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(IR_RX_TIMER, IR_RX_TIMER_CH, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(IR_RX_TIMER, IR_RX_TIMER_CH, LL_TIM_IC_POLARITY_BOTHEDGE);

    LL_DMA_DisableChannel(IR_RX_DMA, IR_RX_DMA_CH);
    IR_RX_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(IR_RX_DMA, IR_RX_DMA_CH, IR_RX_BUFFER_SAMPLES);
    LL_DMA_SetPeriphAddress(IR_RX_DMA, IR_RX_DMA_CH, (uint32_t) &(IR_RX_TIMER->CCR1));
    LL_DMA_SetMemoryAddress(IR_RX_DMA, IR_RX_DMA_CH, (uint32_t) ir_rx_dma_data);
    LL_DMA_EnableChannel(IR_RX_DMA, IR_RX_DMA_CH);

    memset(ir_rx_dma_data, 0, sizeof(ir_rx_dma_data));
    ir_rx_dma_data_read_index = _ir_rx_get_pos();

    IR_RX_LL_TIM_EnableIT_CC();
    LL_TIM_CC_EnableChannel(IR_RX_TIMER, IR_RX_TIMER_CH);
    LL_TIM_EnableCounter(IR_RX_TIMER);
}

void ir_rx_loop() {
    static ir_rx_value last_value = 0;
    static uint32_t last_tick = 0;

    volatile size_t pos = _ir_rx_get_pos();
    while (pos != ir_rx_dma_data_read_index) {
        ir_rx_value value = ir_rx_dma_data[ir_rx_dma_data_read_index++];
        if (ir_rx_dma_data_read_index >= IR_RX_BUFFER_SAMPLES) {
            ir_rx_dma_data_read_index = 0;
        }

        uint32_t t = value - last_value;
        uint32_t current_tick = time_get();
        if ((current_tick - last_tick) < 1000) { // prevent timer overflows
            ir_rx_received(t);
        }
        last_value = value;
        last_tick = current_tick;
    }
}

size_t _ir_rx_get_pos() {
    return (IR_RX_BUFFER_SAMPLES - LL_DMA_GetDataLength(IR_RX_DMA, IR_RX_DMA_CH));
}

void ir_rx_irq() {
    volatile size_t pos = _ir_rx_get_pos();
    pos++;
}