#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // load IDT base
  rtl_li(&s0, cpu.idtr.base);

  // load gate descriptor (offset: s1 -> 0..15, s0 -> 16..31)
  rtl_addi(&s0, &s0, 8 * NO);
  rtl_lm(&s1, &s0, 2);
  rtl_addi(&s0, &s0, 4);
  rtl_lm(&s0, &s0, 4);

  // judge valid bit p
  assert(s0 & 0x00008000);

  // PUSH reg's
  rtl_push(&cpu.eflags);
  cpu.IF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  // construct interupt entry address
  rtl_andi(&s0, &s0, 0xffff0000);
  rtl_add(&s0, &s0, &s1);

  // jump to entry
  rtl_jr(&s0);
}

bool isa_query_intr(void) {
  return false;
}
