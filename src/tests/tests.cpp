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

TEST_CASE("Construction with custom timing")
{
  touchDecoderTimingConfig tc{
      tc.minReleaseTime = 10,
      tc.shortPressTime = 20,
      tc.longPressTime = 30,
      tc.maxIdleShortTime = 50,
      tc.maxIdleLongTime = 50};
  TouchDecoder decoder{tc};
  SECTION("timing in decoder's state machine should match custom timing passed to decoder constructor")
  {
    REQUIRE(decoder.statemachine.tc.minReleaseTime == tc.minReleaseTime);
    REQUIRE(decoder.statemachine.tc.shortPressTime == tc.shortPressTime);
    REQUIRE(decoder.statemachine.tc.longPressTime == tc.longPressTime);
    REQUIRE(decoder.statemachine.tc.maxIdleShortTime == tc.maxIdleShortTime);
    REQUIRE(decoder.statemachine.tc.maxIdleLongTime == tc.maxIdleLongTime);
  }
}

TEST_CASE("decoder - release before min time")
{
  touchDecoderTimingConfig tc{
      tc.minReleaseTime = 10,
      tc.shortPressTime = 20,
      tc.longPressTime = 30,
      tc.maxIdleShortTime = 50,
      tc.maxIdleLongTime = 50};
  TouchDecoder decoder{tc};

  unsigned long current{};

  decoder.push(0, 0);
  decoder.push(1, current += tc.minReleaseTime);
  decoder.push(0, current += (tc.minReleaseTime / 2));
  SECTION("should disregard both previous touch and current release event and return to idle state")
  {
    REQUIRE(decoder.shortPress() == 0);
    REQUIRE(decoder.statemachine.tc.minReleaseTime == tc.minReleaseTime);
  }
}

TEST_CASE("decoder - custom timing - release before min time")
{
  touchDecoderTimingConfig tc{
      tc.minReleaseTime = 20,
      tc.shortPressTime = 25,
      tc.longPressTime = 30,
      tc.maxIdleShortTime = 50,
      tc.maxIdleLongTime = 50};
  TouchDecoder decoder{tc};

  unsigned long current{};

  decoder.push(0, 0);
  decoder.push(1, current += tc.minReleaseTime);
  decoder.push(0, current += (tc.minReleaseTime / 2));
  SECTION("should disregard both previous touch and current release event and return to idle state")
  {
    REQUIRE(decoder.shortPress() == 0);
    REQUIRE(decoder.statemachine.tc.minReleaseTime == tc.minReleaseTime);
  }
}


TEST_CASE("decoder - release within short time - next push after all timeouts expired")
{
  touchDecoderTimingConfig tc{
      tc.minReleaseTime = 10,
      tc.shortPressTime = 20,
      tc.longPressTime = 30,
      tc.maxIdleShortTime = 50,
      tc.maxIdleLongTime = 50};
  TouchDecoder decoder{tc};

  unsigned long current{};

  decoder.push(0, 0);
  decoder.push(1, current += tc.minReleaseTime + 1);
  decoder.push(0, current += tc.shortPressTime - 1);
  decoder.push(0, current += tc.maxIdleShortTime + 1);

  SECTION("should fire short press")
  {
    REQUIRE(decoder.shortPress() == 1);
    REQUIRE(decoder.longPress() == 0);
  }
}

TEST_CASE("decoder - release within long time - next push after all timeouts expired")
{
  touchDecoderTimingConfig tc{
      tc.minReleaseTime = 10,
      tc.shortPressTime = 20,
      tc.longPressTime = 30,
      tc.maxIdleShortTime = 50,
      tc.maxIdleLongTime = 50};
  TouchDecoder decoder{tc};

  unsigned long current{};

  decoder.push(0, 0);
  decoder.push(1, current += tc.minReleaseTime + 1);
  decoder.push(0, current += tc.longPressTime + 1);
  decoder.push(0, current += tc.maxIdleLongTime + 1);

  SECTION("should fire long press")
  {
    REQUIRE(decoder.shortPress() == 0);
    REQUIRE(decoder.longPress() == 1);
  }
}
