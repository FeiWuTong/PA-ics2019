#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // TODO
  rtl_push(&cpu.eflags);
}

bool isa_query_intr(void) {
  return false;
}
