#include <log.h>
#include <platform.h>
#include <printf.h>

#include <arch/spinlock.h>

#include "debug/FramebufferConsole.h"

namespace platform {
extern debug::FramebufferConsole *gConsole;
};

DECLARE_SPINLOCK_S(gPanicLock);

static void _outchar_serial(char ch, void *ctx) { platform_debug_spew(ch); }

static void _outchar_console(char ch, void *ctx) {
  using namespace platform;

  if (gConsole) {
    gConsole->write(ch);
  }
}

static void _outchar_both(char ch, void *ctx) {
  _outchar_serial(ch, ctx);
  _outchar_console(ch, ctx);
}

void log(const char *format, ...) {
  va_list va;
  va_start(va, format);

  auto time = platform_timer_now();
  auto seconds = time & 0xFF;
  auto minutes = (time & 0xFF00) >> 8;
  auto hours = (time & 0xFF0000) >> 16;

  auto disc = format[0];

  auto &func = disc == SERIAL[0] || disc == CSERIAL[0]       ? _outchar_serial
               : disc == TERMINAL[0] || disc == CTERMINAL[0] ? _outchar_console
               : disc == BOTH[0] || disc == CBOTH[0]         ? _outchar_both
                                                             : _outchar_serial;

  if (disc != CSERIAL[0] && disc != CTERMINAL[0] && disc != CBOTH[0])
    fctprintf(func, NULL, "[%2u:%2u:%2u] ", hours, minutes, seconds);

  if (disc == SERIAL[0] || disc == CSERIAL[0] || disc == TERMINAL[0] ||
      disc == CTERMINAL[0] || disc == BOTH[0] || disc == CBOTH[0])
    fctvprintf(func, NULL, &format[1], va);
  else
    fctvprintf(func, NULL, format, va);

  func('\n', nullptr);

  va_end(va);
}

void panic(const char *format, ...) {
  void *pc = __builtin_return_address(0);

  SPIN_LOCK(gPanicLock);

  // set up panic buffer
  constexpr static const size_t kPanicBufSz = 2048;
  static char panicBuf[kPanicBufSz];

  va_list va;
  va_start(va, format);

  vsnprintf(panicBuf, kPanicBufSz, format, va);

  va_end(va);

  fctprintf(_outchar_both, 0, "\033[41mpanic: %s\npc: $%p\033[0m\n", panicBuf,
            pc);

  for (;;)
    asm volatile("cli\nhlt");
}
