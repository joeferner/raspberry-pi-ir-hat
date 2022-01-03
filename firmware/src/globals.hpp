#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#include "hal/Clocks.hpp"
#include "peripheral/USART.hpp"

extern hal::Clocks clocks;
extern peripheral::USART<DEBUG_TX_BUFFER_SIZE, DEBUG_RX_BUFFER_SIZE> debugUsart;
extern peripheral::USART<DEBUG_TX_BUFFER_SIZE, DEBUG_RX_BUFFER_SIZE> rpiUsart;

#endif
