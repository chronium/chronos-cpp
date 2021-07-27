#include <log.h>
#include <platform.h>
#include <printf.h>

#include <arch/spinlock.h>

DECLARE_SPINLOCK_S(gPanicLock);

static void _outchar(char ch, void *ctx) { platform_debug_spew(ch); }
static void _outchar_panic(char ch, void *ctx) {
  platform_debug_spew(ch);

  // TODO
  /*if(gConsole) {
      gConsole->write(ch);
  }*/
}

void log(const char *format, ...) {
  va_list va;
  va_start(va, format);

  auto time = platform_timer_now();
  auto seconds = time & 0xFF;
  auto minutes = (time & 0xFF00) >> 8;
  auto hours = (time & 0xFF0000) >> 16;

  fctprintf(_outchar, NULL, "[%2u:%2u:%2u] ", hours, minutes, seconds);
  fctvprintf(_outchar, NULL, format, va);

  va_end(va);

  platform_debug_spew('\n');
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

  fctprintf(_outchar_panic, 0, "\033[41mpanic: %s\npc: $%p\033[0m\n", panicBuf,
            pc);

  for (;;)
    asm volatile("cli\nhlt");
}
