#include <memory.h>
#include <stdlib.h>

#include "config.h"
#include "current_sensor.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

#define IR_TX_BUFFER_LEN_BEFORE_SEND 10

typedef void (*send_string)(const char *);

void setup();

void setup_system_clock();

void loop();

void process_rx(char *data, send_string send_string);

void setup() {
  time_setup();
  debug_setup();
  rpi_setup();
  ir_rx_setup();
  ir_tx_setup();
  current_sensor_setup();
  debug_send_string("?READY\n");
  LL_IWDG_Enable(IWDG);
}

void loop() {
  LL_IWDG_ReloadCounter(IWDG);
  debug_loop();
  rpi_loop();
  ir_rx_loop();
  current_sensor_loop();
}

void debug_rx(char *data) { process_rx(data, debug_send_string); }

void rpi_rx(char *data) { process_rx(data, rpi_send_string); }

void process_rx(char *data, send_string send_string) {
  if (strcmp(data, "+iwdg") == 0) {
    send_string("+OK\n");
    while (1)
      ;
  } else if (strcmp(data, "+send") == 0) {
    ir_tx_send();
    send_string("+OK\n");
  } else if (strncmp(data, "+f", 2) == 0) {
    uint32_t carrier_freq = atoi(data + 2);
    ir_tx_reset(carrier_freq);
    send_string("+OK\n");
  } else if (strncmp(data, "+s", 2) == 0) {
    char *pon = data + 2;
    char *p = strchr(pon, ',');
    if (p == NULL) {
      send_string("-ERR missing comma\n");
      return;
    }
    *p = '\0';
    uint32_t t_on = atoi(pon);
    uint32_t t_off = atoi(p + 1);
    ir_tx_write(t_on, t_off);
    if (ir_tx_buffer_length() > IR_TX_BUFFER_LEN_BEFORE_SEND) {
      ir_tx_send();
    }
    send_string("+OK\n");
  } else {
    send_string("-ERR \"");
    send_string(data);
    send_string("\"\n");
  }
}

void ir_rx_received(uint32_t value) {
  char buffer[20];
  buffer[0] = '!';
  buffer[1] = 's';
  utoa(value, buffer + 2, 10);
  strcat(buffer, "\n");

  rpi_send_string(buffer);
  debug_send_string(buffer);
}

void current_sensor_overrun_error() {
  const char *str = "-ERR current sensor overrun\n";
  rpi_send_string(str);
  debug_send_string(str);
}