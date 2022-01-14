#include "Denon.hpp"

#include <gtest/gtest.h>

#include "test/test.hpp"

TEST(DenonDecoderTest, DecodePowerOn) {
  ir::Denon decoder;
  ir::DecoderState state;

  readWaveform(state, "denon/power-on.csv");

  ir::DecoderResults results;
  EXPECT_TRUE(decoder.decode(state, &results)) << "should be decoded";
  EXPECT_EQ(ir::Protocol::Denon, results.protocol);
  EXPECT_EQ(15, results.numberOfBits);
  EXPECT_EQ(135, results.command);
  EXPECT_EQ(8, results.address);
  EXPECT_EQ(ir::Endian::MSB, results.endian);
  EXPECT_EQ(false, results.autoRepeat);
  EXPECT_EQ(false, results.parityFailed);
  EXPECT_EQ(false, results.repeat);
}

TEST(DenonDecoderTest, DecodePowerOff) {
  ir::Denon decoder;
  ir::DecoderState state;

  readWaveform(state, "denon/power-off.csv");

  ir::DecoderResults results;
  EXPECT_TRUE(decoder.decode(state, &results)) << "should be decoded";
  EXPECT_EQ(ir::Protocol::Denon, results.protocol);
  EXPECT_EQ(15, results.numberOfBits);
  EXPECT_EQ(71, results.command);
  EXPECT_EQ(8, results.address);
  EXPECT_EQ(ir::Endian::MSB, results.endian);
  EXPECT_EQ(false, results.autoRepeat);
  EXPECT_EQ(false, results.parityFailed);
  EXPECT_EQ(false, results.repeat);
}
