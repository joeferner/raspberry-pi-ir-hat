#include "main.h"

#include <memory.h>
#include <stdlib.h>

#include "hal/ADC.hpp"
#include "hal/Bus.hpp"
#include "hal/Clocks.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/IWDG.hpp"
#include "hal/NVIC.hpp"
#include "hal/RCC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "hal/USART.hpp"
#include "peripheral/CurrentSensor.hpp"
#include "peripheral/IrRx.hpp"
#include "peripheral/IrTx.hpp"
#include "peripheral/USART.hpp"
#include "setup.hpp"
#include "time.h"

hal::System halSystem;
hal::Clocks clocks;
hal::Bus bus;
hal::NVICHal nvic;
hal::RCCHal rcc;

hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin0> currentRefPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin4> current0Pin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin5> current1Pin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin6> irRxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7> irInLedPin;

hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6> usart1TxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7> usart1RxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9> irOutPin;

hal::GPIO<hal::gpio::GPIOAddress::GPIOFAddress, hal::gpio::GPIOPin::Pin2> resetPin;

hal::USART<hal::usart::USARTAddress::USART1Address> usart1;
hal::USART<hal::usart::USARTAddress::USART2Address> usart2;
hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5> irRxDmaChannel;
hal::Timer<hal::timer::TimerAddress::TIM3Address> irRxTimer;
hal::Timer<hal::timer::TimerAddress::TIM17Address> irTxCarrierTimer;
hal::Timer<hal::timer::TimerAddress::TIM16Address> irTxSignalTimer;
hal::ADCHal<hal::adc::ADCAddress::ADC1Address> currentSensorAdc;
hal::IWDGHal iwdg;

peripheral::USART<hal::usart::USARTAddress::USART1Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE> debugUsart(
    &usart1);
peripheral::USART<hal::usart::USARTAddress::USART2Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE> rpiUsart(
    &usart2);
peripheral::IrRx irRx(&irRxDmaChannel, &irInLedPin);
peripheral::IrTx irTx(&irOutPin, &irTxCarrierTimer, &irTxSignalTimer);
peripheral::CurrentSensor currentSensor(&currentSensorAdc);

#define IR_TX_BUFFER_LEN_BEFORE_SEND 10

static void loop();

void processUsartLine(peripheral::USARTWriter& usartWriter, const char* buffer);

extern "C" int main() {
  setup();
  while (1) {
    loop();
  }
}

static void loop() {
  iwdg.reloadCounter();

  char usartBuffer[USART_RX_BUFFER_SIZE];
  size_t lineLen;
  if ((lineLen = debugUsart.readLine(usartBuffer, sizeof(usartBuffer)))) {
    processUsartLine(debugUsart, usartBuffer);
  }
  if ((lineLen = rpiUsart.readLine(usartBuffer, sizeof(usartBuffer)))) {
    processUsartLine(rpiUsart, usartBuffer);
  }

  uint16_t irRxValue;
  while (irRx.read(clocks, &irRxValue)) {
    char buffer[20];
    buffer[0] = '!';
    buffer[1] = 's';
    utoa(irRxValue, buffer + 2, 10);
    strcat(buffer, "\n");

    rpiUsart.write(buffer);
    debugUsart.write(buffer);
    iwdg.reloadCounter();
  }

  // currentSensor.loop();
}

void processUsartLine(peripheral::USARTWriter& usartWriter, const char* data) {
  if (strcmp(data, "+iwdg") == 0) {
    usartWriter.write("+OK\n");
    while (1)
      ;
  } else if (strcmp(data, "+send") == 0) {
    irTx.send();
    usartWriter.write("+OK\n");
  } else if (strncmp(data, "+f", 2) == 0) {
    uint32_t carrierFrequency = atoi(data + 2);
    irTx.reset(carrierFrequency);
    usartWriter.write("+OK\n");
  } else if (strncmp(data, "+s", 2) == 0) {
    const char* pon = data + 2;
    const char* p = strchr(pon, ',');
    if (p == NULL) {
      usartWriter.write("-ERR missing comma\n");
      return;
    }
    char temp[10];
    // TODO verify length and does zero terminate
    strncpy(temp, pon, p - pon);
    uint32_t t_on = atoi(temp);
    uint32_t t_off = atoi(p + 1);
    irTx.write(t_on, t_off);
    if (irTx.getNumberOfSamplesInBuffer() > IR_TX_BUFFER_LEN_BEFORE_SEND) {
      irTx.send();
    }
    usartWriter.write("+OK\n");
  } else if (strncmp(data, "+c", 2) == 0) {
    uint16_t d;
    if (data[2] == '0') {
      d = currentSensor.get(peripheral::CurrentSensorInput::Current0);
    } else if (data[2] == '1') {
      d = currentSensor.get(peripheral::CurrentSensorInput::Current1);
    } else if (data[2] == 'r') {
      d = currentSensor.get(peripheral::CurrentSensorInput::Reference);
    } else {
      usartWriter.write("-ERR invalid channel\n");
      return;
    }
    char buffer[50];
    strcpy(buffer, "+OK ");
    itoa(d, buffer + strlen(buffer), 10);
    strcat(buffer, "\n");
    usartWriter.write(buffer);
  } else {
    usartWriter.write("-ERR \"");
    usartWriter.write(data);
    usartWriter.write("\"\n");
  }
}

void assert_failed(uint8_t* file, uint32_t line) {
  __disable_irq();
  while (1) {
  }
}
