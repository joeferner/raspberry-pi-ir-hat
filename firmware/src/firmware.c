#include <memory.h>
#include "config.h"
#include "debug.h"
#include "rpi.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

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
    debug_send_string("-ERR ");
    debug_tx(data, data_len);
    debug_send_string("\n");
}

void ir_rx_received(uint32_t value) {
    LL_WWDG_SetCounter(WWDG, 64);
    rpi_ir_rx_received(value);

    debug_send_string("P");
    debug_send_uint32(value);
    debug_send_string("\n");
}