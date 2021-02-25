# Touch decoder (e.g. detect short, long, double touches) for use with capacitive touch panel (mpr121) based on boost::sml state machine

The code is being developed and tested on WSL2 (Windows) and targeted for use with an ESP32.

Note: The decoder width (number of touch pads decoded) is set to 12 in the library via ```decoderwidth``` - 12 being the number of touch pads handled by the mpr121 chip.


See "touchdecoder.hpp" (top of file):
```c++
#pragma once

#include <boost/sml.hpp>
#include <bitset>
#include <iostream>
#include <vector>

static const int decoderwidth = 12;
```

STATUS: Work in progress - still in early testing stage: 
- [x] Testing of state machine code without timeout on WSL2
- [x] Reject touch/release events below minimum time difference to help with false alarms due to noise on line
- [x] Implement short touch
- [x] Implement long touch
- [x] Implement short-short (double tap) touch
- [x] Implement long-short touch
- [-] REJECTED: Implement morse code
- [x] Implement dependency of decoder width on single constant ```decoderwidth```
- [ ] Research and implement proper timing (e.g. timing double mouse click?)
- [ ] Test decoder on ESP32
- [ ] (optional) Publish library as a PlatformIO library 
- [ ] tbd

Caution: I am still new at C++, so use with caution: You get to keep the pieces ...

Thanks to
- Kris Jusiak for the boost::sml library (and with it the requirement to learn more about modern C++ syntax)
- ourarash for the template to get started with VSCode, CMake and Catch2 (https://github.com/ourarash/cmake-catch-vsc-template.git)

## Research and implement proper timing

Did not find too many sources, apart from [MSDN - SetDoubleClickTime](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setdoubleclicktime)

-> Will implement some measurement code to get a feel for decent timings
- measure tap - tap times :-)
