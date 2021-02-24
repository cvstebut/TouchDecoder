# Touch decoder (e.g. detect short, long, double touches) for use with capacitive touch panel (mpr121) based on boost::sml state machine

The code is being developed and tested on WSL2 (Windows) and targeted for use with an ESP32.

Note: The decoder is currently hard coded to use an array of 12 state-machines representing the 12 touch pads on common mpr121 boards.
I was yet not able to initialize the state-machine array using a varialbe array length (C++ newbie...). If you know how, please contact me.

STATUS: Work in progress - still in early testing stage: 
- [x] Testing of state machine code without timeout on WSL2
- [x] Reject touch/release events below minimum time difference to help with false alarms due to noise on line
- [x] Implement short touch
- [x] Implement long touch
- [ ] Implement short-short (double tap) touch
- [ ] Implement long-short touch
- [ ] REJECTED: Implement morse code
- [ ] Test decoder on ESP32
- [ ] (optional) Publish library as a PlatformIO library 
- [ ] tbd

Caution: I am still new at C++, so use with caution: You get to keep the pieces ...

Thanks to
- Kris Jusiak for the boost::sml library (and with it the requirement to learn more about modern C++ syntax)
- ourarash for the template to get started with VSCode, CMake and Catch2 (https://github.com/ourarash/cmake-catch-vsc-template.git)

