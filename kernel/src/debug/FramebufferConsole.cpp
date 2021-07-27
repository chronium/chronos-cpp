#include "FramebufferConsole.h"
#include "BitmapFonts.h"

#include <int.h>
#include <log.h>
#include <string.h>

using namespace debug;

constexpr static inline bool IsDigit(const char ch) {
  return (ch >= '0') && (ch <= '9');
}

constexpr static u64 StrToInt(const char *str) {
  u64 i = 0U;

  while (IsDigit(*str))
    i = i * 10U + (u64)((*str++) - '0');

  return i;
}

FramebufferConsole::FramebufferConsole(u32 *fb, const ColorOrder format,
                                       const size w, const size h,
                                       const size stride)
    : buffer(fb), bufferFormat(format) {
  this->bufferSize[Size::Width] = w;
  this->bufferSize[Size::Height] = h;

  if (!stride)
    this->bufferStride = w * sizeof(u32);
  else
    this->bufferStride = stride;

  this->font = &BitmapFontHelper::gFonts[0];

  this->bufferChars[Size::Width] = w / font->width;
  this->bufferChars[Size::Height] = h / font->height;

  this->updateColors();
  this->clear();
}

void FramebufferConsole::write(const char *str, const size_t _length) {
  auto strlen = _length;

  if (!strlen) {
    auto temp = str;
    while (*temp++)
      strlen++;
  }

  for (size i = 0; i < strlen; i++)
    this->write(*str++);
}

void FramebufferConsole::write(const char ch) {
  switch (this->writeState) {
  case WriteState::Idle:
    if (ch != '\033')
      this->print(ch);
    else
      this->writeState = WriteState::AnsiEscapeStart;
    break;

  case WriteState::AnsiEscapeStart:
    if (ch == '[') {
      this->writeState = WriteState::AnsiReadingCSI;

      this->ansiBufUsed = 0;
      memset(this->ansiBuf, 0, kAnsiBufSize);
    } else
      this->writeState = WriteState::Idle;

    break;

  case WriteState::AnsiReadingCSI:
    this->ansiBuf[this->ansiBufUsed++] = ch;

    if (ch >= 0x40 && ch <= 0x7E) {
      this->processAnsi();
      this->writeState = WriteState::Idle;
    } else if (this->ansiBufUsed == kAnsiBufSize)
      this->writeState = WriteState::Idle;

    break;
  }
}

void FramebufferConsole::processAnsi() {
  if (!this->ansiBufUsed)
    return;

  const auto finalByte = this->ansiBuf[this->ansiBufUsed - 1];

  switch (finalByte) {
  case 'm':
    this->processAnsiSgr();
    break;

  case 'H':
    this->processAnsiCup();
    break;

  default:
    log("Unhandled ANSI sequence: '%s' (%lu chars, final $%02x)", this->ansiBuf,
        this->ansiBufUsed, finalByte);
    break;
  }
}

void FramebufferConsole::processAnsiSgr() {
  size numberRead = 0;
  char number[8]{0};
  auto readPtr = this->ansiBuf;

  if (this->ansiBufUsed == 1) {
    this->processAnsiSgr(this->ansiBuf, 0);
    goto beach;
  }

  this->ansiBuf[this->ansiBufUsed - 1] = '\0';

  while (const auto ch = *readPtr++) {
    if (ch == ';') {
      this->processAnsiSgr(number, numberRead);

      memset(number, 0, sizeof(number));
      numberRead = 0;
    } else
      number[numberRead++] = ch;
  }

  if (numberRead)
    this->processAnsiSgr(number, numberRead);

beach:
  this->updateColors();
}

void FramebufferConsole::processAnsiSgr(const char *str, const size strlen) {
  if (!strlen || (strlen == 1 && str[0] == '0')) {
    this->colorIndices[ColorIndex::Foreground] = 0xF;
    this->colorIndices[ColorIndex::Background] = 0x0;

    return;
  }

  auto cmd = StrToInt(str);

  if (cmd >= 30 && cmd <= 37)
    this->colorIndices[ColorIndex::Foreground] = cmd - 30;
  else if (cmd >= 40 && cmd <= 47)
    this->colorIndices[ColorIndex::Background] = cmd - 40;

  if (cmd >= 90 && cmd <= 97)
    this->colorIndices[ColorIndex::Foreground] = cmd - 90 + 8;
  else if (cmd >= 100 && cmd <= 107)
    this->colorIndices[ColorIndex::Background] = cmd - 100 + 8;
}

void FramebufferConsole::processAnsiCup() {
  bool isRow = true;
  size numberRead = 0;
  char number[8]{0};
  auto readPtr = this->ansiBuf;

  this->ansiBuf[this->ansiBufUsed - 1] = '\0';

  while (const auto ch = *readPtr++) {
    if (ch == ';') {
      auto val = StrToInt(number);
      this->cursorPos[isRow ? 1 : 0] = val ? (val - 1) : 0;
      isRow = !isRow;

      memset(number, 0, sizeof(number));
      numberRead = 0;
    } else
      number[numberRead++] = ch;
  }

  auto val = StrToInt(number);
  this->cursorPos[isRow ? 1 : 0] = val ? (val - 1) : 0;
}

void FramebufferConsole::print(const char _ch) {
  if (_ch == '\n')
    return this->newline();

  const auto ch = (_ch > this->font->maxGlyph) ? '?' : _ch;
  this->drawChar(ch, this->cursorPos[0], this->cursorPos[1]);

  if (++this->cursorPos[0] == this->bufferChars[Size::Width])
    this->newline();
}

void FramebufferConsole::drawChar(const char ch, const size x, const size y) {
  const u8 *fontData = this->font->data + (ch * this->font->stride);
  const auto yOff = this->font->height * y * this->bufferStride;

  auto ptr = this->buffer + (x * this->font->width);
  ptr = reinterpret_cast<u32 *>(reinterpret_cast<uptr>(ptr) + yOff);

  // get colors for fg/bg
  const auto fg = this->colors[ColorIndex::Foreground];
  const auto bg = this->colors[ColorIndex::Background];

  // write character
  for (size y = 0; y < this->font->height; y++) {
    auto rowPtr = ptr;

    for (size x = 0; x < this->font->width; x++) {
      const u8 bit = (1 << (x % 8));
      *rowPtr++ = (*fontData & bit) ? fg : bg;

      // advance to the next cell in font data
      if (x && !(x % 8)) {
        fontData++;
      }
    }

    // advance to next line
    fontData++; // XXX: this will break for fonts not multiples of 8 pixels
                // wide!
    ptr = reinterpret_cast<u32 *>(reinterpret_cast<uptr>(ptr) +
                                  this->bufferStride);
  }
}

void FramebufferConsole::newline() {
  this->cursorPos[0] = 0;
  if (++this->cursorPos[1] == this->bufferChars[Size::Height]) {
    // scroll the entire screen up
    const auto yOff = this->font->height * kScrollAmount * this->bufferStride;
    const auto toMove = this->bufferStride *
                        (this->bufferChars[Size::Height] - kScrollAmount) *
                        this->font->height;

    auto toPtr = this->buffer;
    auto fromPtr =
        reinterpret_cast<u32 *>(reinterpret_cast<uptr>(toPtr) + yOff);

    memcpy(toPtr, fromPtr, toMove);

    // clear the newly uncovered area
    const auto bg = this->colors[ColorIndex::Background];
    auto clearPtr =
        reinterpret_cast<u32 *>(reinterpret_cast<uptr>(toPtr) + toMove);

    for (size y = 0; y < (kScrollAmount * this->font->height); y++) {
      auto rowPtr = clearPtr;

      for (size x = 0; x < this->bufferSize[Size::Width]; x++) {
        *rowPtr++ = bg;
      }

      clearPtr = reinterpret_cast<u32 *>(reinterpret_cast<uptr>(clearPtr) +
                                         this->bufferStride);
    }

    // update cursor position
    this->cursorPos[1] -= kScrollAmount;
  }
}

void FramebufferConsole::updateColors() {
  for (size i = 0; i < ColorIndex::Maximum; i++) {
    const auto &color =
        this->palette[this->colorIndices[i] % kColorPaletteEntries];
    this->colors[i] = color.convert(this->bufferFormat);
  }
}

void FramebufferConsole::clear() {
  auto ptr = this->buffer;
  const auto bg = this->colors[ColorIndex::Background];

  for (size y = 0; y < this->bufferSize[Size::Height]; y++) {
    auto rowPtr = ptr;

    for (size x = 0; x < this->bufferSize[Size::Width]; x++) {
      *rowPtr++ = bg;
    }

    // go to next row
    ptr = reinterpret_cast<u32 *>(reinterpret_cast<u64>(ptr) +
                                  this->bufferStride);
  }
}
