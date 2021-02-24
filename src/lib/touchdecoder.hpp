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
};

class state_machine
{
  using Self = state_machine;

public:
  // States
  struct Idle
  {
  };

  struct Touched
  {
    unsigned long entry_time{};
  };

  struct Short
  {
    unsigned long entry_time{};
  };

  struct Long
  {
    unsigned long entry_time{};
  };

  // Events
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

    auto guard_min_time = [](const auto &event, Touched &state) {
      bool timeout = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff < 10)
      {
        timeout = true;
      }
      std::cout << "Key " << event.keyid << " - guard_min_time (" << timeDiff << ") = " << timeout << std::endl;
      return timeout;
    };
    auto guard_short_press_time = [](const auto &event, Touched &state) {
      bool result = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff >= 10 && timeDiff < 20)
      {
        result = true;
      }
      std::cout << "Key " << event.keyid << " - guard_short_press_time (" << timeDiff << ") = " << result << std::endl;
      return result;
    };
    auto guard_long_press_time = [](const auto &event, Touched &state) {
      bool result = false;
      unsigned long timeDiff = event.event_time - state.entry_time;
      if (timeDiff >= 30)
      {
        result = true;
      }
      std::cout << "Key " << event.keyid << " - guard_long_press_time (" << timeDiff << ") = " << result << std::endl;
      return result;
    };

    const auto touch_action = [](const auto &event, Touched & state) {
      std::cout << "Key " << event.keyid << " - Transition due to touch_event" << std::endl;
      state.entry_time = event.event_time;
    };
    const auto release_short_action = [](decoderOutput &dcOutput, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to release_short_action" << std::endl;
      dcOutput.shortPress.set(event.keyid);
    };
    const auto release_long_action = [](decoderOutput &dcOutput, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to release_long_action" << std::endl;
      dcOutput.longPress.set(event.keyid);
    };
    const auto timeout_action = [](decoderOutput &dcOutput, const auto &event) {
      std::cout << "Key " << event.keyid << " - Transition due to timeout_action" << std::endl;
    };
    // clang-format off
    return make_transition_table(
        * state<Idle>      + event<touch_event>                                / (touch_action)         = state<Touched>
        , state<Touched>   + event<release_event>   [guard_min_time]           / (timeout_action)       = state<Idle>
        , state<Touched>   + event<release_event>   [guard_short_press_time]   / (release_short_action) = state<Idle>
        , state<Touched>   + event<release_event>   [guard_long_press_time]    / (release_long_action)  = state<Idle>
    );
    // clang-format on
  }
  bool guard3(int i) const noexcept
  {
    assert(42 == i);
    std::cout << "guard3" << std::endl;
    return true;
  }
};

class TouchDecoder
{
public:
  state_machine statemachine{};
  boost::sml::sm<state_machine> smarray[decoderwidth] = {
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output},
      boost::sml::sm<state_machine>{statemachine, state_machine::Touched{0}, output}};

  void push(uint16_t touchstate, unsigned long pushTime)
  {
    std::bitset<decoderwidth> newState = std::bitset<decoderwidth>(touchstate);
    _released = current & ~newState;
    _touched = ~current & newState;
    output.shortPress = 0;
    output.longPress = 0;
    currenttime = pushTime;

    std::cout << "PRE-SM  - current: " << current << ", touchstate: " << touchstate << ", _released: " << _released << ", _touched: " << _touched << ", shortPress: " << output.shortPress << ", longPress: " << output.longPress << std::endl;

    current = newState;

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

  int width()
  {
    return decoderwidth;
  }

private:
  unsigned long currenttime;
  int empMaxTouched; // max count of touches in buffer. If exceeded will clear buffer and set empDetected = true
  int _empTouchCount = 0;
  int bufSize;

  decoderOutput output;
  std::bitset<decoderwidth> current = 0;
  std::bitset<decoderwidth> _released;
  std::bitset<decoderwidth> _touched;

  // actions
};
