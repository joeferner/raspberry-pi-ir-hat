#include "main.h"

#include <memory.h>
#include <stdlib.h>

#include "BothUSARTWriter.hpp"
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
#include "ir/Decoder.hpp"
#include "ir/Encoder.hpp"
#include "peripheral/CurrentSensor.hpp"
#include "peripheral/IrRx.hpp"
#include "peripheral/IrTx.hpp"
#include "peripheral/USART.hpp"
#include "setup.hpp"
#include "time.h"
#include "utils/Args.hpp"

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
peripheral::IrTx irTx(&clocks, &iwdg, &irOutPin, &irTxCarrierTimer, &irTxSignalTimer);
peripheral::CurrentSensor currentSensor(&currentSensorAdc);

static uint32_t lastIrSignalTime;
static uint32_t lastIrStartOfSignal;
static uint32_t lastIrEndOfSignal;
ir::Decoder irDecoder;
ir::Encoder irEncoder;

#define IR_TX_BUFFER_LEN_BEFORE_SEND 10

static void loop();

static void processUsartLine(peripheral::USARTWriter& usartWriter, const char* buffer);
static void processUsartWatchdog(peripheral::USARTWriter& usartWriter);
static void processUsartIrSend(peripheral::USARTWriter& usartWriter, Args& args);
static void processUsartGetCurrent(peripheral::USARTWriter& usartWriter, Args& args);
static void processUsartInvalidCommand(peripheral::USARTWriter& usartWriter, Args& args);

extern "C" int main() {
  setup();
  lastIrSignalTime = 0;
  lastIrStartOfSignal = 0;

  usartOutput.writef("?READY\n");

  while (1) {
    loop();
  }
}

static void loop() {
  iwdg.reloadCounter();

  char usartBuffer[USART_RX_BUFFER_SIZE];
  if (debugUsart.readLine(usartBuffer, sizeof(usartBuffer))) {
    processUsartLine(debugUsart, usartBuffer);
  }
  if (rpiUsart.readLine(usartBuffer, sizeof(usartBuffer))) {
    processUsartLine(rpiUsart, usartBuffer);
  }

  uint16_t irRxValue;
  while (irRx.read(clocks, &irRxValue)) {
    if (lastIrStartOfSignal == 0) {
      lastIrStartOfSignal = clocks.getTickCount();
    }

    ir::DecoderResults results;
    if (irDecoder.push(irRxValue, &results)) {
      uint32_t delta = (lastIrEndOfSignal == 0) ? 0 : (lastIrStartOfSignal - lastIrEndOfSignal);
      lastIrStartOfSignal = 0;
      usartOutput.writef(
          "!s%d,%d,%d,%d,%d,%d,%d\n",
          (int)results.protocol,
          results.address,
          results.command,
          results.repeat ? 1 : 0,
          results.autoRepeat ? 1 : 0,
          results.parityFailed ? 1 : 0,
          delta);
      lastIrEndOfSignal = clocks.getTickCount();
    }
    lastIrSignalTime = clocks.getTickCount();
  }

  if (lastIrSignalTime != 0 && (clocks.getTickCount() - lastIrSignalTime) > ir::Decoder::MAX_QUITE_TIME_MS) {
    irDecoder.clear();
    lastIrStartOfSignal = 0;
    lastIrEndOfSignal = 0;
    lastIrSignalTime = 0;
  }

  // TODO enable current sensor
  // currentSensor.loop();
}

static void processUsartLine(peripheral::USARTWriter& usartWriter, const char* data) {
  Args args(data);
  if (args.read("+iwdg")) {
    processUsartWatchdog(usartWriter);
  } else if (args.read("+s")) {
    processUsartIrSend(usartWriter, args);
  } else if (args.read("+c")) {
    processUsartGetCurrent(usartWriter, args);
  } else {
    processUsartInvalidCommand(usartWriter, args);
  }
}

void assert_failed(uint8_t* file, uint32_t line) {
  __disable_irq();
  while (1) {
  }
}

static void processUsartWatchdog(peripheral::USARTWriter& usartWriter) {
  usartWriter.write("+OK\n");
  while (1)
    ;
}

static void processUsartIrSend(peripheral::USARTWriter& usartWriter, Args& args) {
  uint32_t protocol;
  if (!args.read(&protocol) || !args.readComma()) {
    processUsartInvalidCommand(usartWriter, args);
    return;
  }

  uint32_t address;
  if (!args.read(&address) || !args.readComma()) {
    processUsartInvalidCommand(usartWriter, args);
    return;
  }

  uint32_t command;
  if (!args.read(&command)) {
    processUsartInvalidCommand(usartWriter, args);
    return;
  }

  uint32_t numberOfRepeats = 0;
  if (args.readComma() && args.read(&numberOfRepeats)) {
    // number of repeats provided
  }

  if (!irEncoder.send(irTx, (ir::Protocol)protocol, address, command, numberOfRepeats)) {
    usartWriter.write("-ERR failed to send\n");
    return;
  }
  usartWriter.write("+OK\n");
}

static void processUsartGetCurrent(peripheral::USARTWriter& usartWriter, Args& args) {
  uint16_t d;
  char c = args.readChar();
  if (c == '0') {
    d = currentSensor.get(peripheral::CurrentSensorInput::Current0);
  } else if (c == '1') {
    d = currentSensor.get(peripheral::CurrentSensorInput::Current1);
  } else if (c == 'r') {
    d = currentSensor.get(peripheral::CurrentSensorInput::Reference);
  } else {
    processUsartInvalidCommand(usartWriter, args);
    return;
  }
  usartWriter.writef("+OK %d\n", d);
}

static void processUsartInvalidCommand(peripheral::USARTWriter& usartWriter, Args& args) {
  const char* buffer = args.getWholeBuffer();
  if (buffer[0] == '\0') {
    usartWriter.write("\n");
    return;
  }
  usartWriter.writef("-ERR \"%s\"\n", buffer);
}
