#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#include "hal/Clocks.hpp"
#include "peripheral/CurrentSensor.hpp"
#include "peripheral/IrRx.hpp"
#include "peripheral/IrTx.hpp"
#include "peripheral/USART.hpp"

extern hal::Clocks clocks;

extern peripheral::USART<hal::usart::USARTAddress::USART1Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE>
    debugUsart;
extern peripheral::USART<hal::usart::USARTAddress::USART2Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE> rpiUsart;

extern peripheral::IrRx irRx;
extern peripheral::IrTx irTx;

extern peripheral::CurrentSensor currentSensor;

#endif
