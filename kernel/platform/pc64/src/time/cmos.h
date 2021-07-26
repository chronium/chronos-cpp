#pragma once

#include <stdint.h>

#define CURRENT_YEAR 2021

namespace platform {
class CMOS {
public:
  enum { ADDRESS = 0x70, DATA = 0x71 };

  static void Read();

  static uint8_t Second();
  static uint8_t Minute();
  static uint8_t Hour();
  static uint8_t Day();
  static uint8_t Month();
  static uint32_t Year();
};
} // namespace platform
