#include "exceptions.h"
#include "exception_types.h"

#include "gdt.h"
#include "idt.h"

#include <log.h>
#include <printf.h>

#include <arch/x86_msr.h>

using namespace arch;

static const struct {
  u8 vector;
  const char *name;
} amd64_exception_name[] = {
    {X86_EXC_DIVIDE, "Divide-by-zero"},
    {X86_EXC_DEBUG, "Debug"},
    {X86_EXC_NMI, "Non-Maskable Interrupt"},
    {X86_EXC_BREAKPOINT, "Breakpoint"},
    {X86_EXC_OVERFLOW, "Overflow"},
    {X86_EXC_BOUNDS, "Bound range exceeded"},
    {X86_EXC_ILLEGAL_OPCODE, "Invalid instruction"},
    {X86_EXC_DEVICE_UNAVAIL, "Device unavailable (FPU)"},
    {X86_EXC_DOUBLE_FAULT, "Double fault"},
    {X86_EXC_INVALID_TSS, "Invalid TSS"},
    {X86_EXC_SEGMENT_NP, "Segment not present"},
    {X86_EXC_SS, "Invalid stack segment"},
    {X86_EXC_GPF, "General protection fault"},
    {X86_EXC_PAGING, "Page fault"},
    {X86_EXC_FP, "Floating point exception"},
    {X86_EXC_ALIGNMENT, "Alignment check"},
    {X86_EXC_MCE, "Machine check"},
    {X86_EXC_SIMD_FP, "SIMD float exception"},
    {X86_EXC_VIRT, "Virtualization exception"},

    // these are ones we should never get
    {15, "Reserved"},
    {0xFF, NULL},
};

static const char *vector_name(const u8 vector) {
  size i = 0;

  while (amd64_exception_name[i].name) {
    if (amd64_exception_name[i].vector == vector)
      return amd64_exception_name[i].name;

    i++;
  }

  return "Unknown";
}

extern "C" void amd64_exception_div0();
extern "C" void amd64_exception_debug();
extern "C" void amd64_exception_nmi();
extern "C" void amd64_exception_breakpoint();
extern "C" void amd64_exception_overflow();
extern "C" void amd64_exception_bounds();
extern "C" void amd64_exception_invalid_instruction();
extern "C" void amd64_exception_device_unavailable();
extern "C" void amd64_exception_double_fault();
extern "C" void amd64_exception_tss_invalid();
extern "C" void amd64_exception_segment_missing();
extern "C" void amd64_exception_ss_invalid();
extern "C" void amd64_exception_gpf();
extern "C" void amd64_exception_pagefault();
extern "C" void amd64_exception_float();
extern "C" void amd64_exception_alignment_check();
extern "C" void amd64_exception_machine_check();
extern "C" void amd64_exception_simd();
extern "C" void amd64_exception_virtualization();

void arch::InstallExceptionHandlers(IDT *idt) {
  idt->set(X86_EXC_DIVIDE, (uintptr_t)amd64_exception_div0, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_DEBUG, (uintptr_t)amd64_exception_debug, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack4);
  idt->set(X86_EXC_NMI, (uintptr_t)amd64_exception_nmi, GDT_KERN_CODE_SEG,
           IDT_FLAGS_ISR, IDT::Stack::Stack3);
  idt->set(X86_EXC_BREAKPOINT, (uintptr_t)amd64_exception_breakpoint,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack4);
  idt->set(X86_EXC_OVERFLOW, (uintptr_t)amd64_exception_overflow,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_BOUNDS, (uintptr_t)amd64_exception_bounds, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_ILLEGAL_OPCODE,
           (uintptr_t)amd64_exception_invalid_instruction, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_DEVICE_UNAVAIL,
           (uintptr_t)amd64_exception_device_unavailable, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_DOUBLE_FAULT, (uintptr_t)amd64_exception_double_fault,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_INVALID_TSS, (uintptr_t)amd64_exception_tss_invalid,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_SEGMENT_NP, (uintptr_t)amd64_exception_segment_missing,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_SS, (uintptr_t)amd64_exception_ss_invalid, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_GPF, (uintptr_t)amd64_exception_gpf, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_PAGING, (uintptr_t)amd64_exception_pagefault,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_FP, (uintptr_t)amd64_exception_float, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_ALIGNMENT, (uintptr_t)amd64_exception_alignment_check,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack2);
  idt->set(X86_EXC_MCE, (uintptr_t)amd64_exception_machine_check,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack4);
  idt->set(X86_EXC_SIMD_FP, (uintptr_t)amd64_exception_simd, GDT_KERN_CODE_SEG,
           IDT_FLAGS_TRAP, IDT::Stack::Stack1);
  idt->set(X86_EXC_VIRT, (uintptr_t)amd64_exception_virtualization,
           GDT_KERN_CODE_SEG, IDT_FLAGS_TRAP, IDT::Stack::Stack1);
}

int amd64_exception_format_info(char *outBuf, const size outBufLen,
                                const amd64_exception_info_t *info) {
  int err;
  u64 cr0, cr2, cr3, cr4;
  asm volatile("mov %%cr0, %0\n\t"
               "mov %%cr2, %1\n\t"
               "mov %%cr3, %2\n\t"
               "mov %%cr4, %3\n\t"
               : "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4));

  uint32_t tempLo, tempHi;
  uint64_t gsBase = 0, fsBase = 0, gsKernBase = 0;

  x86_msr_read(X86_MSR_FSBASE, &tempLo, &tempHi);
  fsBase = (tempLo) | (static_cast<uint64_t>(tempHi) << 32);

  x86_msr_read(X86_MSR_GSBASE, &tempLo, &tempHi);
  gsBase = (tempLo) | (static_cast<uint64_t>(tempHi) << 32);
  x86_msr_read(X86_MSR_KERNEL_GSBASE, &tempLo, &tempHi);
  gsKernBase = (tempLo) | (static_cast<uint64_t>(tempHi) << 32);

  // format
  err = snprintf(outBuf, outBufLen,
                 "Exception %3lu ($%016lx)\n"
                 "CR0 $%016lx CR2 $%016lx CR3 $%016lx CR4 $%016lx\n"
                 " CS $%04lx SS $%04lx RFLAGS $%016lx\n"
                 " FS $%016llx  GS $%016llx KGS $%016llx\n"
                 "RAX $%016llx RBX $%016llx RCX $%016llx RDX $%016llx\n"
                 "RDI $%016llx RSI $%016llx RBP $%016llx RSP $%016llx\n"
                 " R8 $%016llx  R9 $%016llx R10 $%016llx R11 $%016llx\n"
                 "R12 $%016llx R13 $%016llx R14 $%016llx R15 $%016llx\n"
                 "RIP $%016llx\n",
                 info->intNo, info->errCode, cr0, cr2, cr3, cr4, info->cs,
                 info->ss, info->rflags, fsBase, gsBase, gsKernBase, info->rax,
                 info->rbx, info->rcx, info->rdx, info->rdi, info->rsi,
                 info->rbp, info->rsp, info->r8, info->r9, info->r10, info->r11,
                 info->r12, info->r13, info->r14, info->r15, info->rip);

  // specicial cases
  switch (info->intNo) {
  case X86_EXC_SIMD_FP:
    uint32_t mxcsr;
    asm volatile("stmxcsr %0" : "=m"(mxcsr));

    err += snprintf(outBuf + err, outBufLen - err, "MXCSR: $%08x", mxcsr);
    break;
  }

  return err;
}

void amd64_handle_pagefault(amd64_exception_info_t *info) {
  static bool inFault{false};

  if (__atomic_test_and_set(&inFault, __ATOMIC_RELAXED)) {
    panic("nested page fault detected!");
  }

  uint64_t faultAddr;
  asm volatile("mov %%cr2, %0" : "=r"(faultAddr));

  constexpr static const size_t kBufSz = 1024;
  static char buf[kBufSz] = {0};
  amd64_exception_format_info(buf, kBufSz, info);
  panic("unhandled page fault: %s%s %s (%s) at $%016llx\n%s",
        ((info->errCode & 0x08) ? "reserved bit violation on " : ""),
        ((info->errCode & 0x04) ? "user" : "supervisor"),
        ((info->errCode & 0x02) ? "write" : "read"),
        ((info->errCode & 0x01) ? "present" : "not present"), faultAddr, buf);

  // TODO

  for (;;) {
    asm volatile("hlt");
  }
}

void amd64_handle_exception(amd64_exception_info_t *info) {
  // TODO
  constexpr static const size_t kBufSz = 1024;
  char buf[kBufSz] = {0};
  amd64_exception_format_info(buf, kBufSz, info);
  panic("unhandled exception: %s\n%s", vector_name(info->intNo), buf);
}
