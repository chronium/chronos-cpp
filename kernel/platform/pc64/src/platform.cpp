#include "platform.h"
#include "io/serial.h"

#include <debug/FramebufferConsole.h>
#include <int.h>
#include <log.h>
#include <printf.h>
#include <string.h>

inline void *operator new(size_t, void *p) throw() { return p; }

namespace platform {
static u8 gSharedConsole[sizeof(debug::FramebufferConsole)];
debug::FramebufferConsole *gConsole = nullptr;

void PlatformInit(boot_info_t *info) {
  Serial::Init();
  log("Serial initialized");

  FramebufferInit(info);
  log("Framebufer initialized");

  PrintBootMessage(info);
}

void FramebufferInit(boot_info_t *info) {
  using namespace debug;
  using ColorOrder = FramebufferConsole::ColorOrder;

  auto w = info->Framebuffer.W;
  auto h = info->Framebuffer.H;
  auto addr = info->Framebuffer.Addr;
  auto stride = info->Framebuffer.Stride;

  memclr(gSharedConsole, sizeof(FramebufferConsole));

  gConsole = new (gSharedConsole)
      FramebufferConsole(addr, ColorOrder::ARGB, w, h, stride);
}

void PrintBootMessage(boot_info_t *info) {
  int nChars;
  char buf[100]{0};

  constexpr static const size_t kInfoBoxWidth{50};

  nChars = snprintf(buf, sizeof(buf),
                    "\n\nChronOS (built on %s) - "
                    "Copyright 2021 Chronium\n\n",
                    __DATE__);
  gConsole->write(buf, nChars);

  // top info box
  gConsole->write(" \x05", 2);
  for (size_t i = 0; i < kInfoBoxWidth; i++) {
    gConsole->write(0x01);
  }
  gConsole->write("\x06\n", 2);

  gConsole->write(" \x00", 2);
  nChars = snprintf(buf, sizeof(buf), "Willkommen zu ChronOS");
  gConsole->write(buf, nChars);

  for (size_t i = nChars; i < kInfoBoxWidth; i++) {
    gConsole->write(' ');
  }
  gConsole->write("\x00\n", 2);

  gConsole->write(" \x00", 2);
  nChars = snprintf(buf, sizeof(buf), "Available Memory: %llu MB",
                    info->AvailableMemory / 1024 / 1024);
  gConsole->write(buf, nChars);

  for (size_t i = nChars; i < kInfoBoxWidth; i++) {
    gConsole->write(' ');
  }
  gConsole->write("\x00\n", 2);

  // bottom info box
  gConsole->write(" \x04", 2);
  for (size_t i = 0; i < kInfoBoxWidth; i++) {
    gConsole->write(0x01);
  }
  gConsole->write("\x03\n", 2);
}
}; // namespace platform
