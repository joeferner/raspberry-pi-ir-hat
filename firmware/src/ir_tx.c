#include <stm32f0xx_ll_gpio.h>
#include "config.h"
#include "ir_tx.h"

void ir_tx_setup() {
    LL_GPIO_InitTypeDef gpio_init = {0};

//    HAL_TIM_OC_Start(&IR_TX_TIMER_CARRIER, IR_TX_TIMER_CARRIER_CH);
//    HAL_TIM_OC_Start(&IR_TX_TIMER_SIGNAL, IR_TX_TIMER_SIGNAL_CH);
//
//    gpio_init.Pin = LL_GPIO_PIN_9;
//    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
//    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//    gpio_init.Pull = LL_GPIO_PULL_NO;
//    gpio_init.Alternate = LL_GPIO_AF_0;
//    LL_GPIO_Init(GPIOB, &gpio_init);
}
