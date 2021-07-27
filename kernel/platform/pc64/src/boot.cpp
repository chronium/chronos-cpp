#include <debug/FramebufferConsole.h>
#include <platform/stivale2.h>
#include <printf.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "io/serial.h"
#include "platform.h"

#include <log.h>

using namespace platform;

namespace platform {
extern debug::FramebufferConsole *gConsole;
}

static uint8_t stack[1024 * 16];

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = 0},
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"),
               used)) static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2),
    .tags = (uintptr_t)&framebuffer_hdr_tag};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
  struct stivale2_tag *current_tag =
      static_cast<stivale2_tag *>((void *)stivale2_struct->tags);
  for (;;) {
    if (current_tag == NULL) {
      return NULL;
    }

    if (current_tag->identifier == id) {
      return current_tag;
    }

    current_tag = static_cast<stivale2_tag *>((void *)current_tag->next);
  }
}

void arch_init();

void kinit();

static size memSize;

// The following will be our kernel's entry point.
extern "C" void _start(struct stivale2_struct *stivale2_struct) {
  struct stivale2_struct_tag_framebuffer *framebuffer;
  framebuffer = static_cast<stivale2_struct_tag_framebuffer *>(
      stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
  if (framebuffer == NULL) {
    // It wasn't found, just hang...
    for (;;) {
      asm("hlt");
    }
  }

  struct stivale2_struct_tag_memmap *mmap;
  mmap = static_cast<stivale2_struct_tag_memmap *>(
      stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID));

  for (int i = 0; i < mmap->entries; i++) {
    auto &entry = mmap->memmap[i];
    if (entry.type == STIVALE2_MMAP_USABLE)
      memSize += entry.length;
  }

  boot_info_t boot_info = {
      .Framebuffer = {.W = framebuffer->framebuffer_width,
                      .H = framebuffer->framebuffer_height,
                      .Stride = framebuffer->framebuffer_pitch,
                      .Addr = reinterpret_cast<u32 *>(
                          framebuffer->framebuffer_addr)},
      .AvailableMemory = memSize,
  };

  PlatformInit(&boot_info);

  arch_init();

  kinit();

  // We're done, just hang...
  for (;;) {
    asm("hlt");
  }
}
