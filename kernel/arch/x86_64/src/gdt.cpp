#include "gdt.h"

#include <log.h>
#include <string.h>

extern "C" void amd64_gdt_flush();

using namespace arch;

gdt_descriptor_t GDT::gGdt[GDT::kGdtSize] __attribute__((aligned(64)));

amd64_tss_t gBspTss;

// XXX: these should be made larger to match the stack pool size
/// Size of the IRQ stack (in 8 byte units)
constexpr static const size_t kIrqStackSz = 512;

static __attribute__((aligned(64))) uintptr_t gBspIrqStacks[7][kIrqStackSz];

bool GDT::gLogLoad = true;
bool GDT::gLogSet = true;

size_t GDT::gTssIndex = 1;

void GDT::Init() {
  memset(gGdt, 0, sizeof(gdt_descriptor_t) * kGdtSize);

  Set32((GDT_KERN_CODE_SEG >> 3), 0x00000000, 0xFFFFFFFF, 0b10011010,
        0b00100000);
  Set32((GDT_KERN_DATA_SEG >> 3), 0x00000000, 0xFFFFFFFF, 0b10010110, 0xCF);

  Set32((GDT_USER_CODE_SEG >> 3), 0x00000000, 0xFFFFFFFF, 0b11111010,
        0b00100000);
  Set32((GDT_USER_CODE64_SEG >> 3), 0x00000000, 0xFFFFFFFF, 0b11111010,
        0b00100000);

  Set32((GDT_USER_DATA_SEG >> 3), 0x00000000, 0xFFFFFFFF, 0b11110010, 0);

  InitTss(&gBspTss);

  for (size_t i = 0; i < 7; i++) {
    const auto stack = reinterpret_cast<uintptr_t>(&gBspIrqStacks[i]) +
                       (kIrqStackSz * sizeof(uintptr_t));

    gBspTss.ist[i].low = stack & 0xFFFFFFFF;
    gBspTss.ist[i].high = stack >> 32ULL;
  }

  InstallTss(0, &gBspTss);

  Load();
  ActivateTask(0);
}

void GDT::ActivateTask(const size task) {
  const uint16_t sel = ((task * 2) * 8) + GDT_FIRST_TSS;

  if (gLogLoad)
    log("Load task: %04x", sel);
  asm volatile("ltr %0" : : "r"(sel) : "memory");
}

void GDT::Set32(const size num, const u32 base, const u32 limit, const u8 flags,
                const u8 gran) {
  REQUIRE(num <= (GDT_USER_CODE64_SEG / 8), "%u-bit GDT index out of range: %u",
          32, num);

  gGdt[num].base_low = (base & 0xFFFF);
  gGdt[num].base_middle = (base >> 16) & 0xFF;
  gGdt[num].base_high = (base >> 24) & 0xFF;

  gGdt[num].limit_low = (limit & 0xFFFF);
  gGdt[num].granularity = (limit >> 16) & 0x0F;

  gGdt[num].granularity |= gran & 0xF0;
  gGdt[num].access = flags;

  if (gLogSet)
    log("GDT %4x: %08x base: %08x limit: %08x flags: %02x gran: %02x", num,
        *(u32 *)&gGdt[num], base, limit, flags, gran);
}

void GDT::Set64(const size num, const uptr base, const u32 limit,
                const u8 flags, const u8 granularity) {
  REQUIRE(num >= (GDT_USER_CODE64_SEG / 8), "%u-bit GDT index out of range: %u",
          64, num);

  gdt_descriptor64_t desc;
  memset(&desc, 0, sizeof(desc));

  desc.limit0 = (limit & 0xFFFF);
  desc.granularityLimit = (limit >> 16) & 0x0F;

  desc.base0 = (base & 0xFFFF);
  desc.base1 = (base >> 16) & 0xFF;
  desc.base2 = (base >> 24) & 0xFF;
  desc.base3 = (base >> 32ULL);

  desc.typeFlags = flags;

  desc.granularityLimit |= (granularity & 0xF0);

  memcpy(&gGdt[num], &desc, sizeof(desc));

  if (gLogSet)
    log("GDT %4x: %016llx", num, *(u64 *)&desc);
}

void GDT::Load(const size numTss) {
  struct {
    uint16_t length;
    uint64_t base;
  } __attribute__((__packed__)) GDTR;

  GDTR.length = (GDT_FIRST_TSS + (numTss * 16)) - 1;
  GDTR.base = reinterpret_cast<uintptr_t>(&gGdt);
  asm volatile("lgdt (%0)" : : "r"(&GDTR) : "memory");

  if (gLogLoad)
    log("Load GDT %p len %u", GDTR.base, GDTR.length);

  amd64_gdt_flush();
}

void GDT::InitTss(amd64_tss_t *tss) {
  memset(tss, 0, sizeof(amd64_tss_t));

  tss->ioMap = (sizeof(amd64_tss_t) - 1);
}

void GDT::InstallTss(const size i, amd64_tss_t *tss) {
  Set64((i * 2) + (GDT_FIRST_TSS / 8), reinterpret_cast<uintptr_t>(tss),
        sizeof(amd64_tss_t) - 1, 0x89, 0);
}
