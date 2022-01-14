#ifndef _TEST_HPP_
#define _TEST_HPP_

#include <stdint.h>

#ifdef TEST
void assert_failed(uint8_t* file, uint32_t line);

#define __disable_irq() \
  {}
#define __enable_irq() \
  {}
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t*)__FILE__, __LINE__))

namespace ir {
class DecoderState;
}

void readWaveform(ir::DecoderState& state, const char* filename);
#else
#include "main.h"
#endif

#endif
