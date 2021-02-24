#pragma once

#include <boost/circular_buffer.hpp>

#include <boost/sml.hpp>
#include <bitset>
#include <cassert>
#include <iostream>
#include <typeinfo>

static const int decoderwidth = 12;
struct decoderOutput
{
  std::bitset<decoderwidth> shortPress{};
  std::bitset<decoderwidth> longPress{};
  std::bitset<decoderwidth> doubleShortPress{};
  std::bitset<decoderwidth> doubleLongPress{};
  std::bitset<decoderwidth> shortLongPress{};
};

struct touchDecoderTimingConfig
{
  unsigned long minReleaseTime{};
  unsigned long shortPressTime{};
  unsigned long longPressTime{};
  unsigned long maxIdleShortTime{};
  unsigned long maxIdleLongTime{};
};

class state_machine
{
  using Self = state_machine;

public:
  explicit state_machine(const touchDecoderTimingConfig &tc) : tc{tc} {}

  // States
  struct Idle
  {
  };

  struct IdleShort
  {
    unsigned long entry_time{};
    int keyid{};
  };

  struct IdleLong
  {
    unsigned long entry_time{};
    int keyid{};
  };

  struct Touched
  {
    unsigned long entry_time{};
  };

  struct TouchedSecond
  {
    unsigned long entry_time{};
  };

  // Events
  struct no_event
  {
    int keyid{};
    unsigned long event_time{};
  };

  struct touch_event
  {
    int keyid{};
    unsigned long event_time{};
  };
  struct release_event
  {
    int keyid{};
    unsigned long event_time{};
  };
  // Transition table
  auto operator()() const
  {
    using namespace boost::sml;

    auto guard_min_touch_time = [this](const auto &event, Touched &state) {
      bool timeout = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff < tc.minReleaseTime)
      {
        timeout = true;
      }
      std::cout << "Key " << event.keyid << " - guard_min_touch_time (" << timeDiff << ") = " << timeout << std::endl;
      return timeout;
    };
    auto guard_short_press_time = [this](const auto &event, Touched &state) {
      bool result = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff >= tc.minReleaseTime && timeDiff < tc.shortPressTime)
      {
        result = true;
      }
      std::cout << "Key " << event.keyid << " - guard_short_press_time (" << timeDiff << ") = " << result << std::endl;
      return result;
    };
    auto guard_long_press_time = [this](const auto &event, Touched &state) {
      bool result = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff >= tc.longPressTime)
      {
        result = true;
      }
      std::cout << "Key " << event.keyid << " - guard_long_press_time (" << timeDiff << ") = " << result << std::endl;
      return result;
    };

    auto guard_max_idleshort_time = [this](const auto &event, IdleShort &state, unsigned long &currentTime) {
      bool timeout = false;
      unsigned long timeDiff = currentTime - state.entry_time;
      if (timeDiff > tc.maxIdleShortTime)
      {
        timeout = true;
      }
      std::cout << "Key " << state.keyid << " - guard_max_idleshort_time (" << timeDiff << ") = " << timeout << std::endl;
      return timeout;
    };

    auto guard_max_idlelong_time = [this](const auto &event, IdleLong &state, unsigned long &currentTime) {
      bool timeout = false;
      unsigned long timeDiff = currentTime - state.entry_time;
      if (timeDiff > tc.maxIdleLongTime)
      {
        timeout = true;
      }
      std::cout << "Key " << state.keyid << " - guard_max_idlelong_time (" << timeDiff << ") = " << timeout << std::endl;
      return timeout;
    };

    const auto touch_action = [](const auto &event, Touched &state) {
      std::cout << "Key " << event.keyid << " - Transition due to touch_event" << std::endl;
      state.entry_time = event.event_time;
    };

    const auto release_to_idle_short = [](decoderOutput &dcOutput, IdleShort &state, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to release_to_idle_short" << std::endl;
      state.entry_time = event.event_time;
      state.keyid = event.keyid;
    };

    const auto release_to_idle_long = [](decoderOutput &dcOutput, IdleLong &state, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to release_to_idle_long" << std::endl;
      state.entry_time = event.event_time;
      state.keyid = event.keyid;
    };

    const auto release_short_action = [](decoderOutput &dcOutput, IdleShort &state, const auto &event) {
      std::cout << "Key " << state.keyid << " - Transition due to release_short_action" << std::endl;
      dcOutput.shortPress.set(state.keyid);
    };

    const auto release_long_action = [](decoderOutput &dcOutput, IdleLong &state, const auto &event) {
      std::cout << "Key " << state.keyid << " - Transition due to release_long_action" << std::endl;
      dcOutput.longPress.set(state.keyid);
    };

    const auto timeout_action = [](decoderOutput &dcOutput, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to timeout_action" << std::endl;
    };

    // clang-format off
    return make_transition_table(
        * state<Idle>              + event<touch_event>                                  / (touch_action)          = state<Touched>
        , state<Touched>           + event<release_event>   [guard_min_touch_time]       / (timeout_action)        = state<Idle>
        , state<Touched>           + event<release_event>   [guard_short_press_time]     / (release_to_idle_short) = state<IdleShort>
        , state<Touched>           + event<release_event>   [guard_long_press_time]      / (release_to_idle_long)  = state<IdleLong>
        , state<IdleShort>                                  [guard_max_idleshort_time]   / (release_short_action)  = state<Idle>
        , state<IdleLong>                                   [guard_max_idlelong_time]    / (release_long_action)   = state<Idle>
    );
    // clang-format on
  }

  touchDecoderTimingConfig tc{};
};

class TouchDecoder
{
public:
  explicit TouchDecoder(): statemachine(state_machine{touchDecoderTimingConfig{10, 20, 30, 50, 50}}){}
  explicit TouchDecoder(touchDecoderTimingConfig);

  state_machine statemachine;

  boost::sml::sm<state_machine> smarray[decoderwidth] = {
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output, currentTime}};

  void push(uint16_t touchstate, unsigned long pushTime)
  {
    std::bitset<decoderwidth> newState = std::bitset<decoderwidth>(touchstate);
    _released = current & ~newState;
    _touched = ~current & newState;

    // reset output event registers to zero
    output = decoderOutput{};

    std::cout << "PRE-SM  - current: " << current << ", touchstate: " << touchstate << ", _released: " << _released << ", _touched: " << _touched << ", shortPress: " << output.shortPress << ", longPress: " << output.longPress << std::endl;

    current = newState;
    currentTime = pushTime;

    for (int i = 0; i < decoderwidth; i++)
    {
      if (_touched.test(i))
      {
        smarray[i].process_event(state_machine::touch_event{i, pushTime});
      }
    }

    for (int i = 0; i < decoderwidth; i++)
    {
      if (_released.test(i))
      {
        smarray[i].process_event(state_machine::release_event{i, pushTime});
      }
    }

    for (int i = 0; i < decoderwidth; i++)
    {
      if (~_released.test(i) && ~_touched.test(i))
      {
        std::cout << "Key " << i << " - processing no_event to enable timeout handling without released or touched" << std::endl;
        smarray[i].process_event(state_machine::no_event{i, pushTime});
      }
    }
    std::cout << "POST-SM - current: " << current << ", touchstate: " << touchstate << ", _released: " << _released << ", _touched: " << _touched << ", shortPress: " << output.shortPress << ", longPress: " << output.longPress << std::endl;
  }

  std::bitset<decoderwidth> released()
  {
    return _released;
  }

  std::bitset<decoderwidth> shortPress()
  {
    return output.shortPress;
  }

  std::bitset<decoderwidth> longPress()
  {
    return output.longPress;
  }

  std::bitset<decoderwidth> doubleShortPress()
  {
    return output.doubleShortPress;
  }

  std::bitset<decoderwidth> doubleLongPress()
  {
    return output.doubleLongPress;
  }

  std::bitset<decoderwidth> shortLongPress()
  {
    return output.shortLongPress;
  }

  int width()
  {
    return decoderwidth;
  }

private:
  unsigned long currentTime;
  int empMaxTouched; // max count of touches in buffer. If exceeded will clear buffer and set empDetected = true
  int _empTouchCount = 0;
  int bufSize;

  touchDecoderTimingConfig tc{10, 20};

  decoderOutput output;
  std::bitset<decoderwidth> current = 0;
  std::bitset<decoderwidth> _released;
  std::bitset<decoderwidth> _touched;

  // actions
};

TouchDecoder::TouchDecoder(touchDecoderTimingConfig tc) : statemachine{tc} {}