#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  rtl_lr(&s0, R_ESP, 4);
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&s0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&s0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_sr(R_ESP, &cpu.ebp, 4);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
	rtl_sext(&s0, &cpu.eax, 2);
	rtl_shri(&s0, &s0, 16);
	rtl_sr(R_DX, &s0, 2);
  }
  else {
	rtl_sari(&cpu.edx, &cpu.eax, 31);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
	rtl_lr(&s0, R_AL, 1);
	rtl_sext(&s0, &s0, 1);
	rtl_sr(R_AX, &s0, 2);
  }
  else {
	rtl_sext(&cpu.eax, &cpu.eax, 2);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(movsb) {
  rtl_lm(&s0, &cpu.esi, 1);
  rtl_sm(&cpu.edi, &s0, 1);
  cpu.edi++;
  cpu.esi++;

  print_asm_template2(movsb);
}

make_EHelper(movsv) {
  if (decinfo.isa.is_operand_size_16) rtl_li(&s0, 2);
  else rtl_li(&s0, 4);
  rtl_lm(&s1, &cpu.esi, s0);
  rtl_sm(&cpu.edi, &s1, s0);
  rtl_add(&cpu.edi, &cpu.edi, &s0);
  rtl_add(&cpu.esi, &cpu.esi, &s0);

  print_asm_template2(movsv);
}
