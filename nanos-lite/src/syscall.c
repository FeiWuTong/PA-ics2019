#include "common.h"
#include "syscall.h"

uintptr_t sys_yield() {
  _yield();
  return 0;
}

void sys_exit(int code) {
  _halt(code);
}

size_t sys_write(int fd, const void *buf, size_t count) {
  size_t success = 0;
  // test for sbrk
  //Log("SYS_write are called.");
  switch (fd) {
	case 1:
	case 2:
	  while (success < count) _putc(*((char *)buf + success++));
	  return success;
	default:
	  panic("Unhandled sys_write fd = %d", fd);
	  return 0;
  }
}

int sys_brk(void *addr) {
  // PA3 do not realize sys_brk in fact, just return 0 in fake
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
	case SYS_yield: c->GPRx = sys_yield(); break;
	case SYS_exit: sys_exit(a[1]); break;
	case SYS_write: c->GPRx = sys_write(a[1], (void *)a[2], a[3]); break;
	case SYS_brk: c->GPRx = sys_brk((void *)a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
