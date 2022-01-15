#include "NEC.hpp"

#include <gtest/gtest.h>

#include "test/test.hpp"

TEST(NECDecoderTest, DecodePioneerNum1) {
  ir::NEC decoder;
  ir::DecoderState state;

  readWaveform(state, "pioneer/num1.csv");

  ir::DecoderResults results;
  EXPECT_TRUE(decoder.decode(state, &results)) << "should be decoded";
  EXPECT_EQ(ir::Protocol::NEC, results.protocol);
  EXPECT_EQ(32, results.numberOfBits);
  EXPECT_EQ(1, results.command);
  EXPECT_EQ(170, results.address);
  EXPECT_EQ(ir::Endian::MSB, results.endian);
  EXPECT_EQ(false, results.autoRepeat);
  EXPECT_EQ(false, results.parityFailed);
  EXPECT_EQ(false, results.repeat);
}

TEST(NECDecoderTest, DecodePioneerNum2) {
  ir::NEC decoder;
  ir::DecoderState state;

  readWaveform(state, "pioneer/num2.csv");

  ir::DecoderResults results;
  EXPECT_TRUE(decoder.decode(state, &results)) << "should be decoded";
  EXPECT_EQ(ir::Protocol::NEC, results.protocol);
  EXPECT_EQ(32, results.numberOfBits);
  EXPECT_EQ(2, results.command);
  EXPECT_EQ(170, results.address);
  EXPECT_EQ(ir::Endian::MSB, results.endian);
  EXPECT_EQ(false, results.autoRepeat);
  EXPECT_EQ(false, results.parityFailed);
  EXPECT_EQ(false, results.repeat);
}

TEST(NECDecoderTest, DecodePioneerPower) {
  ir::NEC decoder;
  ir::DecoderState state;

  readWaveform(state, "pioneer/power.csv");

  ir::DecoderResults results;
  EXPECT_TRUE(decoder.decode(state, &results)) << "should be decoded";
  EXPECT_EQ(ir::Protocol::NEC, results.protocol);
  EXPECT_EQ(32, results.numberOfBits);
  EXPECT_EQ(28, results.command);
  EXPECT_EQ(170, results.address);
  EXPECT_EQ(ir::Endian::MSB, results.endian);
  EXPECT_EQ(false, results.autoRepeat);
  EXPECT_EQ(false, results.parityFailed);
  EXPECT_EQ(false, results.repeat);
}
