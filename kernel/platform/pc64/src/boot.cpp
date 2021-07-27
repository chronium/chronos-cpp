#include <debug/FramebufferConsole.h>
#include <platform/stivale2.h>
#include <printf.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "io/serial.h"

#include <log.h>

namespace platform {
/// shared framebuffer console
static u8 gSharedConsole[sizeof(debug::FramebufferConsole)];
debug::FramebufferConsole *gConsole = nullptr;
}; // namespace platform

using namespace platform;

// We need to tell the stivale bootloader where we want our stack to be.
// We are going to allocate our stack as an uninitialised array in .bss.
static uint8_t stack[4096];

// stivale2 uses a linked list of tags for both communicating TO the
// bootloader, or receiving info FROM it. More information about these tags
// is found in the stivale2 specification.

// stivale2 offers a runtime terminal service which can be ditched at any
// time, but it provides an easy way to print out to graphical terminal,
// especially during early boot.
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    // All tags need to begin with an identifier and a pointer to the next tag.
    .tag =
        {// Identification constant defined in stivale2.h and the specification.
         .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
         // If next is 0, it marks the end of the linked list of header tags.
         .next = 0},
    // The terminal header tag possesses a flags field, leave it as 0 for now
    // as it is unused.
    .flags = 0};

// We are now going to define a framebuffer header tag, which is mandatory when
// using the stivale2 terminal.
// This tag tells the bootloader that we want a graphical framebuffer instead
// of a CGA-compatible text mode. Omitting this tag will make the bootloader
// default to text mode, if available.
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag =
        {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
         // Instead of 0, we now point to the previous header tag. The order in
         // which header tags are linked does not matter.
         .next = (uint64_t)&terminal_hdr_tag},
    // We set all the framebuffer specifics to 0 as we want the bootloader
    // to pick the best it can.
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"),
               used)) static struct stivale2_header stivale_hdr = {
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = 0,
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want.
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the
    // executable's
    // segments.
    .flags = (1 << 1) | (1 << 2),
    // This header structure is the root of the linked list of header tags and
    // points to the first one in the linked list.
    .tags = (uintptr_t)&framebuffer_hdr_tag};

// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
  struct stivale2_tag *current_tag =
      static_cast<stivale2_tag *>((void *)stivale2_struct->tags);
  for (;;) {
    // If the tag pointer is NULL (end of linked list), we did not find
    // the tag. Return NULL to signal this.
    if (current_tag == NULL) {
      return NULL;
    }

    // Check whether the identifier matches. If it does, return a pointer
    // to the matching tag.
    if (current_tag->identifier == id) {
      return current_tag;
    }

    // Get a pointer to the next tag in the linked list and repeat.
    current_tag = static_cast<stivale2_tag *>((void *)current_tag->next);
  }
}

typedef void (*term_write)(const char *string, size_t length);

void platform_init(struct stivale2_struct_tag_framebuffer *framebuffer);
void arch_init();

void PrintBootMessage();

void init_framebuffer(struct stivale2_struct_tag_framebuffer *framebuffer);

void kinit();

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

  // Let's get the address of the terminal write function.

  // Now, let's assign this pointer to a function pointer which
  // matches the prototype described in the stivale2 specification for
  // the stivale2_term_write function.

  platform_init(framebuffer);
  arch_init();

  kinit();

  // We're done, just hang...
  for (;;) {
    asm("hlt");
  }
}

void platform_init(struct stivale2_struct_tag_framebuffer *framebuffer) {
  Serial::Init();

  init_framebuffer(framebuffer);

  PrintBootMessage();
}

inline void *operator new(size_t, void *p) throw() { return p; }

void init_framebuffer(struct stivale2_struct_tag_framebuffer *framebuffer) {
  using namespace debug;
  using ColorOrder = FramebufferConsole::ColorOrder;

  auto w = framebuffer->framebuffer_width;
  auto h = framebuffer->framebuffer_height;
  auto addr = framebuffer->framebuffer_addr;

  memclr(gSharedConsole, sizeof(FramebufferConsole));

  gConsole = new (gSharedConsole)
      FramebufferConsole(reinterpret_cast<u32 *>(addr), ColorOrder::ARGB, w, h,
                         framebuffer->framebuffer_pitch);
}

void PrintBootMessage() {
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

  // CPUs (TODO: use the actual number of started cores)
  gConsole->write(" \x00", 2);
  nChars = snprintf(buf, sizeof(buf), "Willkommen zu ChronOS");
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
