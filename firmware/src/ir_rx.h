#ifndef IR_RX_H
#define IR_RX_H

#include "stdint.h"

void ir_rx_setup();

void ir_rx_loop();

extern void ir_rx_received(uint32_t value);

void ir_rx_irq();

#endif
