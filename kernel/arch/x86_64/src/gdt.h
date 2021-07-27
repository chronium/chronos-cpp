#pragma once

#define GDT_KERN_CODE_SEG 0x08
#define GDT_KERN_DATA_SEG 0x10
#define GDT_USER_CODE_SEG 0x18
#define GDT_USER_DATA_SEG 0x20
#define GDT_USER_CODE64_SEG 0x28

#define GDT_FIRST_TSS 0x30

#ifndef ASM_FILE

#include <int.h>

typedef struct arch_gdt_descriptor {
  u16 limit_low;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 granularity;
  u8 base_high;
} __attribute__((packed)) gdt_descriptor_t;

typedef struct arch_gdt_descriptor_64 {
  /// limit 15..0
  u16 limit0;
  /// base 15..0
  u16 base0;
  /// base 23..16
  u8 base1;
  /// present flag, DPL, type
  u8 typeFlags;
  /// granularity, available flag, bits 19..16 of limit
  u8 granularityLimit;
  /// base address 31..24
  u8 base2;
  /// base address 63..32
  u32 base3;
  /// reserved (always zero)
  u32 reserved;
} __attribute__((packed)) gdt_descriptor64_t;

typedef struct amd64_tss {
  u32 reserved1;

  // stack pointers (RSP0 - RSP2)
  struct {
    u32 low, high;
  } __attribute__((packed)) rsp[3];

  u32 reserved2[2];

  // interrupt stacks
  struct {
    u32 low, high;
  } __attribute__((packed)) ist[7];

  u32 reserved3[2];
  u16 reserved4;

  // IO map offset (should be sizeof(amd64_tss) as we don't use it). use high
  // word
  u16 ioMap;
} __attribute__((packed)) amd64_tss_t;

namespace arch {
class GDT {
public:
  static void Init();

  static void AllocTss(amd64_tss_t *&outTss, size &outTssIdx,
                       const bool load = false);
  static void InitTss(amd64_tss_t *tss);
  static void InstallTss(const size i, amd64_tss_t *tss);

private:
  static void Set32(const size idx, const u32 base, const u32 limit,
                    const u8 flags, const u8 granularity);

  static void Set64(const size idx, const uptr base, const u32 limit,
                    const u8 flags, const u8 granularity);

  static void Load(const size numTss = 1);
  static void ActivateTask(const size task);

private:
  constexpr static const size kGdtSize = 128;
  constexpr static const size kNumTssSlots = (kGdtSize - (GDT_FIRST_TSS / 8));

  static gdt_descriptor_t gGdt[kGdtSize];

  static size gTssIndex;

  static bool gLogLoad;
  static bool gLogSet;
};
} // namespace arch

#endif
