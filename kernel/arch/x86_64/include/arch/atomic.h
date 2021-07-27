#pragma once

#include <int.h>

// RW barrier
#define barrier() asm volatile("" : : : "memory")

#define atomic_xadd(P, V) __sync_fetch_and_add((P), (V))
//#define atomic_xadd(P, V) __atomic_fetch_add((P), (V), __ATOMIC_ACQ_REL)
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1)
#define atomic_add(P, V) __sync_add_and_fetch((P), (V))
#define atomic_set_bit(P, V) __sync_or_and_fetch((P), 1 << (V))
#define atomic_clear_bit(P, V) __sync_and_and_fetch((P), ~(1 << (V)))

/// Atomic 32-bit exchange
static inline __attribute__((always_inline)) u32 xchg_32(volatile void *ptr,
                                                         u32 x) {
  asm volatile("xchgl %0, %1"
               : "=r"(x)
               : "m"(*(volatile u32 *)ptr), "0"(x)
               : "memory");
  return x;
}

/// Atomic 16-bit exchange
static inline __attribute__((always_inline)) u16 xchg_16(void *ptr, u16 x) {
  asm volatile("xchgw %0,%1"
               : "=r"(x)
               : "m"(*(volatile u16 *)ptr), "0"(x)
               : "memory");

  return x;
}
