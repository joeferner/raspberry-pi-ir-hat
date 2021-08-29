#include <memory.h>
#include <stdlib.h>

#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

typedef void (*send_string)(const char *);

void setup();

void setup_system_clock();

void loop();

void process_rx(const char *data, send_string send_string);

void setup() {
  time_setup();
  debug_setup();
  rpi_setup();
  ir_rx_setup();
  ir_tx_setup();
  debug_send_string("READY\n");
  LL_IWDG_Enable(IWDG);
}

void loop() {
  LL_IWDG_ReloadCounter(IWDG);
  debug_loop();
  rpi_loop();
  ir_rx_loop();
}

void debug_rx(const char *data) { process_rx(data, debug_send_string); }

void rpi_rx(const char *data) { process_rx(data, rpi_send_string); }

void process_rx(const char *data, send_string send_string) {
  if (strcmp(data, "+iwdg") == 0) {
    send_string("+OK\n");
    while (1)
      ;
  } else {
    send_string("-ERR \"");
    send_string(data);
    send_string("\"\n");
  }
}

void ir_rx_received(uint32_t value) {
  char buffer[15];
  buffer[0] = 'P';
  utoa(value, buffer + 1, 10);
  strcat(buffer, "\n");

  rpi_send_string(buffer);
  debug_send_string(buffer);
}
