#include "gdt.h"
#include "idt.h"

using namespace arch;

void arch_init() {
  GDT::Init();
  IDT::Init();
}
