#pragma once

#include <int.h>

namespace debug {
struct BitmapFont;

class FramebufferConsole {
public:
  enum class ColorOrder {
    RGBA,
    ARGB,
  };

  struct Color {
    Color() : r(0), g(0), b(0), a(0xFF) {}
    Color(const u8 _r, const u8 _g, const u8 _b, const u8 _a = 0xFF)
        : r(_r), g(_g), b(_b), a(_a) {}

    u32 convert(const ColorOrder o) const {
      switch (o) {
      case ColorOrder::RGBA:
        return (this->r << 24) | (this->g << 16) | (this->b << 8) | this->a;
      case ColorOrder::ARGB:
        return (this->a << 24) | (this->r << 16) | (this->g << 8) | this->b;
      }
    }

    u8 r, g, b, a;
  };

public:
  FramebufferConsole(u32 *fb, const ColorOrder format, const size w,
                     const size h, const size stride = 0);

  void write(const char);
  void write(const char *, const size = 0);

  void clear();

private:
  enum Size {
    Width = 0,
    Height = 1,
  };

  enum ColorIndex {
    Foreground = 0,
    Background = 1,

    Maximum
  };

  enum class WriteState {
    Idle,
    AnsiEscapeStart,
    AnsiReadingCSI,
  };

private:
  void updateColors();

  void processAnsi();
  void processAnsiSgr();
  void processAnsiSgr(const char *, const size);
  void processAnsiCup();

  void print(const char);
  void drawChar(const char, const size x, const size y);
  void newline();

private:
  constexpr static const size kColorPaletteEntries = 16;
  constexpr static const size kAnsiBufSize = 32;

  constexpr static const size kScrollAmount = 5;

private:
  u32 *buffer;
  size bufferSize[2];
  size bufferStride;
  ColorOrder bufferFormat;

  size bufferChars[2];

  u8 colorIndices[ColorIndex::Maximum] = {0xF, 0x0};
  u32 colors[ColorIndex::Maximum];

  // clang-format off
  Color palette[kColorPaletteEntries] = {
    // black,             red,                  green,               yellow
    Color(0, 0, 0),       Color(205, 49, 49),   Color(13, 188, 121), Color(229, 229, 16),
    // blue,              magenta,              cyan,                white
    Color(36, 114, 200),  Color(188, 63, 188),  Color(17, 168, 205), Color(229, 229, 229),
    // gray,              bright red,           bright green,        bright yellow
    Color(102, 102, 102), Color(241, 76, 76),   Color(35, 209, 139), Color(245, 245, 67),
    // bright blue,       bright magenta,       bright cyan,         bright white
    Color(59, 142, 234),  Color(214, 112, 214), Color(41, 184, 219), Color(229, 229, 229),
  };
  // clang-format on

  size cursorPos[2] = {0, 0};

  const BitmapFont *font;

  WriteState writeState = WriteState::Idle;
  size ansiBufUsed = 0;
  char ansiBuf[kAnsiBufSize];
};
} // namespace debug
