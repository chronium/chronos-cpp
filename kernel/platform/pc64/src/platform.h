#pragma once

#include <int.h>
#include <platform/stivale2.h>

namespace platform {

typedef struct framebuffer_info {
  u16 W;
  u16 H;
  u32 Stride;
  u32 *Addr;
} framebuffer_info_t;

typedef struct boot_info {
  framebuffer_info_t Framebuffer;

  size AvailableMemory;
} boot_info_t;

void PlatformInit(boot_info_t *);
void FramebufferInit(boot_info_t *);
void PrintBootMessage(boot_info_t *);

} // namespace platform
