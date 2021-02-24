#pragma once

#include <boost/circular_buffer.hpp>

#include <boost/sml.hpp>
#include <bitset>
#include <cassert>
#include <iostream>
#include <typeinfo>

static const int decoderwidth = 12;

class state_machine
{
  using Self = state_machine;
public:
  // Transition table
  auto operator()() const
  {
    using namespace boost::sml;

    const auto touch_action = [](std::bitset<decoderwidth> &single,
                                 const auto &event) { /// event is deduced, order is not important
                                                      //      d.i = event.keyid + 42;
      std::cout << "Key " << event.keyid << " - Transition due to touch_event" << std::endl;
    };
    const auto release_action = [](std::bitset<decoderwidth> &single,
                                   const auto &event) { /// event is deduced, order is not important
                                                        //      d.i = event.keyid + 42;
      std::cout << "Key " << event.keyid << " - Transition due to release_event" << std::endl;
      single.set(event.keyid);
    };
    return make_transition_table(
        *"idle"_s + event<touch_event> / (touch_action) = "touched"_s,
        "touched"_s + event<release_event> / (release_action) = "idle"_s);
  }

  // Events
  struct touch_event
  {
    int keyid{};
  };
  struct release_event
  {
    int keyid{};
  };
};

class TouchDecoder
{
public:
  boost::sml::sm<state_machine> smarray[decoderwidth] = {
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single},
      boost::sml::sm<state_machine>{_single}};

  void push(uint16_t touchstate, unsigned long pushTime)
  {
    std::bitset<decoderwidth> newState = std::bitset<decoderwidth>(touchstate);
    _released = current & ~newState;
    _touched = ~current & newState;
    _single = 0;

    std::cout << "PRE-SM  - current: " << current << ", touchstate: " << touchstate << ", _released: " << _released << ", _touched: " << _touched << ", _single: " << _single << std::endl;

    current = newState;

    for (int i = 0; i < decoderwidth; i++)
    {
      if (_touched.test(i))
      {
        smarray[i].process_event(state_machine::touch_event{i});
      }
    }

    for (int i = 0; i < decoderwidth; i++)
    {
      if (_released.test(i))
      {
        smarray[i].process_event(state_machine::release_event{i});
      }
    }
    std::cout << "POST-SM - current: " << current << ", touchstate: " << touchstate << ", _released: " << _released << ", _touched: " << _touched << ", _single: " << _single << std::endl;
  }

  std::bitset<decoderwidth> released()
  {
    return _released;
  }

  std::bitset<decoderwidth> single()
  {
    return _single;
  }

  int width()
  {
    return decoderwidth;
  }

private:
  int empMaxTouched; // max count of touches in buffer. If exceeded will clear buffer and set empDetected = true
  int _empTouchCount = 0;
  int bufSize;
  std::bitset<decoderwidth> current = 0;
  std::bitset<decoderwidth> _released;
  std::bitset<decoderwidth> _touched;
  std::bitset<decoderwidth> _single;

  // actions
};
