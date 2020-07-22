#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  int i;
  for (i = R_EAX; i <= R_EDI; i++)
	if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) return false;
  if (ref_r->pc != cpu.pc) return false;
  return true;
}

void isa_difftest_attach(void) {
}
