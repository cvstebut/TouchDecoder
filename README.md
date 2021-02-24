# Touch decoder for capacitive touch panel based on boost::sml state machine

STATUS: Work in progress - still in early testing stage: 
- [x] Testing of state machine code without timeout on WSL2
- [ ] Test decoder without timeout on ESP32
- [ ] Implement / test timeout
- [ ] Reject touch/release events below minimum time difference to help with false alarms due to noise on line
- [ ] Implement long touch
- [ ] Implement single-double touch
- [ ] Implement single-long touch
- [ ] tbd

Caution: I am still new at C++, so use with caution: You get to keep the pieces ...

Thanks to
- Kris Jusiak for the boost::sml library (and with it the requirement to learn more about modern C++ syntax)
- ourarash for the template to get started with VSCode, CMake and Catch2 (https://github.com/ourarash/cmake-catch-vsc-template.git)

