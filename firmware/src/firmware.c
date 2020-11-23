#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

static uint32_t ir_tx_signal_length = 8;
static uint32_t ir_tx_signal[] = {
    100, 300,
    200, 100,
    100, 500,
    200, 0
};

void setup();

void setup_system_clock();

void loop();

void setup() {
    debug_setup();
    ir_rx_setup();
    ir_tx_setup();
    debug_send_string("READY\n");
}

void loop() {
    debug_loop();
    ir_rx_loop();
}

void debug_rx(const uint8_t *data, size_t data_len) {
    debug_send_string("OK ");
    debug_tx(data, data_len);
    debug_send_string("\n");
    ir_tx_send(NEC_CARRIER_FREQ, ir_tx_signal, ir_tx_signal_length);
}

void ir_rx_received(ir_rx_value value) {
    debug_send_string("P");
    debug_send_uint32(value);
    debug_send_string("\n");
}