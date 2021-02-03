#ifndef IR_TX_H
#define IR_TX_H

#include <stdint.h>
#include <stddef.h>

#define NEC_CARRIER_FREQ 38000

void ir_tx_setup();

void ir_tx_send(uint32_t carrier_freq, const uint32_t *signal, size_t signal_length);

void ir_tx_irq();

#endif
