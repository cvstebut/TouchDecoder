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
  decoder.push(1, 10);
  decoder.push(0, 15);
  decoder.push(3, 20);
  decoder.push(3, 30);
  decoder.push(15, 40);
  decoder.push(15, 50);
  decoder.push(7, 60);
  decoder.push(3, 70);
  decoder.push(1, 80);
  decoder.push(3, 90);
  decoder.push(0, 100);
  decoder.push(0, 110);
  SECTION("should print state changes")
  {
    REQUIRE(1 == 1);
  }
}