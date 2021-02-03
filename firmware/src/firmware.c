#include <memory.h>
#include "config.h"
#include "debug.h"
#include "rpi.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

#define MAX_SIGNAL_LENGTH 100
uint32_t signal[MAX_SIGNAL_LENGTH];

void setup();

void setup_system_clock();

void loop();

void rpi_rx_tx_ir(const rpi_packet_tx_ir *signal_packet);

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

void rpi_rx(const rpi_packet_header *packet) {
    debug_send_string("RPI RX\n");
    switch (packet->type) {
        case RPI_PACKET_TX_IR:
            rpi_rx_tx_ir((const rpi_packet_tx_ir *) packet);
            break;
        default: {
            debug_send_string("invalid packet type\n");
            break;
        }
    }
}

void rpi_rx_tx_ir(const rpi_packet_tx_ir *signal_packet) {
    if (signal_packet->signal_length > MAX_SIGNAL_LENGTH) {
        debug_send_string("signal too long\n");
        return;
    }
    memcpy(signal, ((uint8_t *) signal_packet) + sizeof(rpi_packet_tx_ir), signal_packet->signal_length * sizeof(uint32_t));
    ir_tx_send(signal_packet->frequency, signal, signal_packet->signal_length);
}

void ir_rx_received(ir_rx_value value) {
    rpi_ir_rx_received(value);

    debug_send_string("P");
    debug_send_uint32(value);
    debug_send_string("\n");
}