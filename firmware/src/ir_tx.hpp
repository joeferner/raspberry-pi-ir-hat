#ifndef IR_TX_H
#define IR_TX_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NEC_CARRIER_FREQ 38000

void ir_tx_setup();

void ir_tx_reset(uint32_t carrier_freq);

void ir_tx_write(const uint32_t t_on, const uint32_t t_off);

void ir_tx_send();

void ir_tx_irq();

size_t ir_tx_buffer_length();

#ifdef __cplusplus
}
#endif

#endif
