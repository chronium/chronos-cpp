#include <log.h>
#include <platform.h>
#include <printf.h>

static void _outchar(char ch, void *ctx) { platform_debug_spew(ch); }

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
  for (;;)
    asm volatile("cli\nhlt");
}