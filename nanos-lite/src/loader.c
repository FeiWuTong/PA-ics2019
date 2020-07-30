#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr eh;
  Elf_Phdr ph;
  int i;
  size_t ph_offset;
  ph_offset = fs_read(fd, &eh, sizeof(Elf_Ehdr));

  for (i = 0; i < eh.e_phnum; i++) {
	ph_offset += fs_read(fd, &ph, eh.e_phentsize);
	if (ph.p_type == PT_LOAD) {
	  fs_lseek(fd, ph.p_offset, SEEK_SET);
	  fs_read(fd, (void *)ph.p_vaddr, ph.p_filesz);
	  if (ph.p_memsz > ph.p_filesz) memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
	  fs_lseek(fd, ph_offset, SEEK_SET);
	} else {
	  break;
	}
  }
  fs_close(fd);
  return eh.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
