#include "idt.h"
#include "exceptions.h"
#include "gdt.h"

#include <log.h>
#include <new>
#include <string.h>

using namespace arch;

static uint8_t gSharedBuf[sizeof(IDT)] __attribute__((aligned(64)));
IDT *arch::gBspIdt = nullptr;

bool IDT::gLogLoad = true;
bool IDT::gLogSet = true;

void IDT::Init() {
  gBspIdt = reinterpret_cast<IDT *>(&gSharedBuf);
  new (gBspIdt) IDT;
}

IDT::IDT() {
  memclr(this->storage, sizeof(idt_entry64_t) * kNumIdt);
  InstallExceptionHandlers(this);

  this->load();
}

void IDT::set(const size entry, const uptr function, const uptr segment,
              const uptr flags, Stack stack) {
  REQUIRE(entry < kNumIdt, "IDT index out of bounds: %u", entry);

  if (gLogSet)
    log("IDT %p index %3u: addr $%016llx segment %04x flags %02x stack %u",
        this, entry, function, segment, flags, (uint8_t)stack);

  this->storage[entry].offset1 = function & 0xFFFF;
  this->storage[entry].offset2 = function >> 16;
  this->storage[entry].selector = segment;
  this->storage[entry].ist = static_cast<u8>(stack);
  this->storage[entry].flags = flags; // OR with 0x60 for user level
  this->storage[entry].offset3 = function >> 32ULL;
}

void IDT::load() {
  struct {
    u16 length;
    u64 base;
  } __attribute__((__packed__)) IDTR;

  IDTR.length = (sizeof(idt_entry64_t) * kNumIdt) - 1;
  IDTR.base = reinterpret_cast<uptr>(&this->storage);

  asm volatile("lidt (%0)" : : "r"(&IDTR));

  if (gLogLoad)
    log("Loaded IDT %p len %u", IDTR.base, IDTR.length);
}
