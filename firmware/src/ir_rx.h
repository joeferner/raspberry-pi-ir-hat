#ifndef IR_RX_H
#define IR_RX_H

#include "stdint.h"

typedef uint32_t ir_rx_value;

void ir_rx_setup();

void ir_rx_loop();

extern void ir_rx_received(ir_rx_value value);

void ir_rx_irq();

#endif
