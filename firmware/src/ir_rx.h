#ifndef _ir_rx_h_
#define _ir_rx_h_

#include "stdint.h"

typedef uint32_t ir_rx_value;

void ir_rx_setup();

void ir_rx_loop();

extern void ir_rx_received(ir_rx_value value);

#endif
