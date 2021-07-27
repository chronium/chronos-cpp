#define ASM_FILE
#include "gdt.h"

.global amd64_gdt_flush
amd64_gdt_flush:
  pushq $GDT_KERN_CODE_SEG
  pushq $1f
  lretq

1:
  mov         $GDT_KERN_DATA_SEG, %ax
  mov         %ax, %ds
  mov         %ax, %es
  mov         %ax, %fs
  mov         %ax, %gs
  mov         %ax, %ss

  ret
