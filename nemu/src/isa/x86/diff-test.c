#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  int i;
  for (i = R_EAX; i <= R_EDI; i++)
	if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) return false;
  if (ref_r->pc != cpu.pc) return false;
  return true;
}

//extern void difftest_memcpy_from_dut(paddr_t, void *, size_t);

void isa_difftest_attach(void) {
  // idtr
  ref_difftest_memcpy_from_dut(0x7e00, (void *)&cpu.idtr.limit, 2);
  ref_difftest_memcpy_from_dut(0x7e02, (void *)&cpu.idtr.base, 4);
  /*
  cpu.idtr.base = 0x1234, cpu.idtr.limit = 0x56;
  difftest_memcpy_from_dut(0x200900, (void *)&cpu.idtr.limit, 2);
  difftest_memcpy_from_dut(0x200902, (void *)&cpu.idtr.base, 4);
  */
  uint8_t lidt[3] = {0x0f, 0x01, 0x18};
  ref_difftest_memcpy_from_dut(0x7e40, (void *)&lidt, sizeof(lidt));
  //difftest_memcpy_from_dut(0x100021, (void *)&lidt, sizeof(lidt));
  CPU_state ref_r;
  ref_r.pc = 0x7e40;
  ref_r.eax = 0x7e00;
  ref_difftest_setregs(&ref_r);
  ref_difftest_exec(1);
  /*
  ref_difftest_getregs(&ref_r);
  printf("idtr.base : 0x%x, idtr.limit : 0x%x\n", cpu.idtr.base, cpu.idtr.limit);
  printf("current pc in REF: 0x%x\n", ref_r.pc);
  */

  // reg (include eflags)
  ref_difftest_setregs((void *)&cpu);

  // pmem
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  ref_difftest_memcpy_from_dut(IMAGE_START, guest_to_host(IMAGE_START), PMEM_SIZE - IMAGE_START);
}

void isa_take_snapshot(char *filename) {
  FILE *fp;
  fp = fopen(filename, "wb");
  fwrite((void *)&cpu, sizeof(cpu), 1, fp);
  fwrite(guest_to_host(0), 1, PMEM_SIZE, fp);
  fclose(fp);
}

void isa_recover_snapshot(char *filename) {
  FILE *fp;
  fp = fopen(filename, "rb");
  fread((void *)&cpu, sizeof(cpu), 1, fp);
  fread(guest_to_host(0), 1, PMEM_SIZE, fp);
  fclose(fp);
}
