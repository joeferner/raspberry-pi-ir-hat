#ifndef IR_RX_H
#define IR_RX_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void ir_rx_setup();

void ir_rx_loop();

extern void ir_rx_received(uint32_t value);

void ir_rx_irq();

#ifdef __cplusplus
}
#endif

#endif
