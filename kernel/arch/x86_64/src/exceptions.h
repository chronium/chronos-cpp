#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <int.h>
#include <x86intrin.h>

typedef struct amd64_exception_info {
  u64 r15, r14, r13, r12, r11, r10, r9, r8;
  u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;

  u64 intNo;
  u64 errCode;

  u64 rip, cs, rflags;
  u64 rsp, ss;
} __attribute__((packed)) amd64_exception_info_t;

void amd64_handle_pagefault(amd64_exception_info_t *info);

void amd64_handle_exception(amd64_exception_info_t *info);

#ifdef __cplusplus
}
#endif

int amd64_exception_format_info(char *outBuf, const size outBufLen,
                                const amd64_exception_info_t *info);

namespace arch {
class IDT;

void InstallExceptionHandlers(IDT *idt);
} // namespace arch
