#include "gdt.h"

using namespace arch;

void arch_init() { GDT::Init(); }
