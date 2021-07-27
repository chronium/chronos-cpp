#pragma once

#include <int.h>

namespace debug {
struct BitmapFont {
  const char *name = nullptr;

  const size width, height;
  const size stride;
  const size maxGlyph;

  const u8 *data = nullptr;
};

struct BitmapFontHelper {
  constexpr static const size kNumFonts = 1;

  static const BitmapFont gFonts[kNumFonts];

  BitmapFontHelper() = delete;
};
} // namespace debug
