#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_ll_bus.h>
#include <stm32f0xx_ll_tim.h>
#include "config.h"
#include "ir_tx.h"

uint32_t signal_length = 1;
uint32_t signal[] = {10, 30, 20, 10, 100, 500};
uint32_t signal_idx = 0;

void next_signal();

void ir_tx_setup() {
    LL_GPIO_InitTypeDef gpio_init = {0};
    LL_TIM_InitTypeDef tim_init = {0};
    LL_TIM_OC_InitTypeDef tim_oc_init = {0};
    LL_TIM_BDTR_InitTypeDef tim_bdtr_init = {0};

    // init IR out
    gpio_init.Pin = IR_TX_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = IR_TX_AF;
    LL_GPIO_Init(IR_TX_PORT, &gpio_init);

    // init carrier timer
    uint32_t carrier_counter = 220;

    LL_APB1_GRP2_EnableClock(IR_TX_CARRIER_PERIPH_TIMER);
    tim_init.Prescaler = 0;
    tim_init.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim_init.Autoreload = carrier_counter;
    tim_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    tim_init.RepetitionCounter = 0;
    LL_TIM_Init(IR_TX_CARRIER_TIMER, &tim_init);
    LL_TIM_DisableARRPreload(IR_TX_CARRIER_TIMER);
    LL_TIM_OC_EnablePreload(IR_TX_CARRIER_TIMER, IR_TX_CARRIER_CHANNEL);
    tim_oc_init.OCMode = LL_TIM_OCMODE_PWM1;
    tim_oc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    tim_oc_init.OCNState = LL_TIM_OCSTATE_DISABLE;
    tim_oc_init.CompareValue = (carrier_counter / 4); // 25% duty cycle
    tim_oc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    tim_oc_init.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    tim_oc_init.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    tim_oc_init.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(IR_TX_CARRIER_TIMER, IR_TX_CARRIER_CHANNEL, &tim_oc_init);
    LL_TIM_OC_DisableFast(IR_TX_CARRIER_TIMER, IR_TX_CARRIER_CHANNEL);
    tim_bdtr_init.OSSRState = LL_TIM_OSSR_DISABLE;
    tim_bdtr_init.OSSIState = LL_TIM_OSSI_DISABLE;
    tim_bdtr_init.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    tim_bdtr_init.DeadTime = 0;
    tim_bdtr_init.BreakState = LL_TIM_BREAK_DISABLE;
    tim_bdtr_init.BreakPolarity = LL_TIM_BREAK_POLARITY_LOW;
    tim_bdtr_init.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(IR_TX_CARRIER_TIMER, &tim_bdtr_init);
    LL_TIM_EnableAllOutputs(IR_TX_CARRIER_TIMER);

    // init signal timer
    LL_APB1_GRP2_EnableClock(IR_TX_SIGNAL_PERIPH_TIMER);
    tim_init.Prescaler = 0;
    tim_init.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim_init.Autoreload = signal[0];
    tim_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    tim_init.RepetitionCounter = 0;
    LL_TIM_Init(IR_TX_SIGNAL_TIMER, &tim_init);
    LL_TIM_DisableARRPreload(IR_TX_SIGNAL_TIMER);
    LL_TIM_OC_DisablePreload(IR_TX_SIGNAL_TIMER, IR_TX_SIGNAL_CHANNEL);
    tim_oc_init.OCMode = LL_TIM_OCMODE_PWM1; // LL_TIM_OCMODE_FORCED_INACTIVE;
    tim_oc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    tim_oc_init.OCNState = LL_TIM_OCSTATE_DISABLE;
    tim_oc_init.CompareValue = signal[0] / 2;
    tim_oc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    tim_oc_init.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    tim_oc_init.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    tim_oc_init.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(IR_TX_SIGNAL_TIMER, IR_TX_SIGNAL_CHANNEL, &tim_oc_init);
    LL_TIM_OC_DisableFast(IR_TX_SIGNAL_TIMER, IR_TX_SIGNAL_CHANNEL);
    tim_bdtr_init.OSSRState = LL_TIM_OSSR_DISABLE;
    tim_bdtr_init.OSSIState = LL_TIM_OSSI_DISABLE;
    tim_bdtr_init.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    tim_bdtr_init.DeadTime = 0;
    tim_bdtr_init.BreakState = LL_TIM_BREAK_DISABLE;
    tim_bdtr_init.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    tim_bdtr_init.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(IR_TX_SIGNAL_TIMER, &tim_bdtr_init);
    LL_TIM_EnableAllOutputs(IR_TX_SIGNAL_TIMER);
    LL_TIM_EnableIT_UPDATE(IR_TX_SIGNAL_TIMER);

    next_signal();

    NVIC_EnableIRQ(TIM16_IRQn);
    NVIC_SetPriority(TIM16_IRQn, 0);
}

void ir_tx_send() {
    signal_idx = 0;
    next_signal();

    LL_TIM_EnableCounter(IR_TX_CARRIER_TIMER);
    LL_TIM_GenerateEvent_UPDATE(IR_TX_CARRIER_TIMER);

    LL_TIM_EnableCounter(IR_TX_SIGNAL_TIMER);
    LL_TIM_GenerateEvent_UPDATE(IR_TX_SIGNAL_TIMER);
}

void next_signal() {
    if (LL_TIM_IsActiveFlag_UPDATE(IR_TX_SIGNAL_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(IR_TX_SIGNAL_TIMER);

        uint32_t on_t = signal[signal_idx] * 800;
        uint32_t off_t = signal[signal_idx + 1] * 800;
        uint32_t total_t = on_t + off_t;

        LL_TIM_SetAutoReload(IR_TX_SIGNAL_TIMER, total_t);
        LL_TIM_OC_SetCompareCH1(IR_TX_SIGNAL_TIMER, on_t);
        signal_idx += 2;
        if (signal_idx >= signal_length) {
            signal_idx = 0;
            LL_TIM_DisableCounter(IR_TX_CARRIER_TIMER);
            LL_TIM_DisableCounter(IR_TX_SIGNAL_TIMER);
        }
    }
}

void ir_tx_irq() {
    next_signal();
}