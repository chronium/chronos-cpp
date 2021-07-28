#pragma once

#include <int.h>

#define IDT_FLAGS_ISR 0x8E
#define IDT_FLAGS_TRAP 0x8F

namespace arch {
typedef struct arch_idt_descriptor_64 {
  u16 offset1;
  u16 selector;
  u8 ist;
  u8 flags;
  u16 offset2;
  u32 offset3;
  u32 reserved;
} __attribute__((packed)) idt_entry64_t;

/**
 * Interface to the 64-bit interrupt descriptor table. We allocate storage for
 * all 256 possible vectors, although we likely won't use them all.
 *
 * TODO: ensure this is placed in per-processor data
 */

class IDT {
  constexpr static const size kNumIdt = 256;

public:
  enum class Stack : u8 {
    /// Do not use an interrupt stack
    None = 0,
    /// First interrupt stack: exceptions
    Stack1 = 1,
    /// Second interrupt stack: faults
    Stack2 = 2,
    /// Third interrupt stack: NMI
    Stack3 = 3,
    /// Fourth interrupt stack: MCE/Debug
    Stack4 = 4,
    /// Fifth interrupt stack: IPIs
    Stack5 = 5,
    /// Sixth interrupt stack: General IRQs
    Stack6 = 6,
    Stack7 = 7,
  };

public:
  static void Init();
  IDT();

  void set(const size entry, const uptr addr, const uptr seg, const uptr flags,
           const Stack stack = Stack::None);

  void load();

private:
  static idt_entry64_t gIdts[kNumIdt];

  static bool gLogLoad;
  static bool gLogSet;

private:
  idt_entry64_t storage[kNumIdt] __attribute__((aligned(64)));
};

extern IDT *gBspIdt;
} // namespace arch
