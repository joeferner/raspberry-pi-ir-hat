#include "config.h"
#include "debug.h"
#include "rpi.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

static uint32_t ir_tx_signal_length = 8;
static uint32_t ir_tx_signal[] = {
    5000, 400,
    1000, 1000,
    2000, 2000,
    5000, 0
};

void setup();

void setup_system_clock();

void loop();

void setup() {
    time_setup();
    debug_setup();
    rpi_setup();
    ir_rx_setup();
    ir_tx_setup();
    debug_send_string("READY\n");
}

void loop() {
    LL_WWDG_SetCounter(WWDG, 64);
    debug_loop();
    rpi_loop();
    ir_rx_loop();
}

void debug_rx(const uint8_t *data, size_t data_len) {
    debug_send_string("-ERR\n");
}

void rpi_rx(const uint8_t *data, size_t data_len) {
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