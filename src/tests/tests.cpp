#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "touchdecoder.hpp"

TEST_CASE("Construction ok")
{
  TouchDecoder decoder;

  SECTION("basic initialization tests should pass")
  {
    REQUIRE(decoder.width() == decoderwidth);
  }
}

TEST_CASE("try some pushing")
{
  TouchDecoder decoder;
  std::bitset<decoderwidth> bs;

  decoder.push(0, 0);
  decoder.push(3, 0);
  decoder.push(3, 0);
  decoder.push(15, 0);
  decoder.push(15, 0);
  decoder.push(7, 0);
  decoder.push(3, 0);
  decoder.push(1, 0);
  decoder.push(3, 0);
  decoder.push(0, 0);
  decoder.push(0, 0);
  SECTION("should print state changes")
  {
    REQUIRE(1 == 1);
  }
}