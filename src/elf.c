#include <linux/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "../utils/utils.h"
#include "../include/elf.h"

static __s8 init_elf(elf_t *elf);

__s8 load_elf(elf_t *elf, __u8 *fname) {
	struct stat st = {0};
	if (stat(fname, &st) == -1)
		return -1;

	int fd = open(fname, O_RDONLY);
	if (fd == -1) return -1;

	elf->size = st.st_size;
	elf->map = mmap(0, elf->size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!elf->map) return -1;

	close(fd);
	return init_elf(elf);
}

static __s8 init_elf(elf_t *elf) {
	Elf64_Ehdr *e = elf->ehdr;
	if (check_elf(elf) == -1) return -1;

	if (!!e->e_phoff) {
		elf->phdr	= elf->map + e->e_phoff;
		elf->ph_num	= e->e_phnum;
	}

	if (!!e->e_shoff) {
		elf->sec	 = elf->map + e->e_shoff;
		elf->sec_num = e->e_shnum;
		elf->sec_strtab = (elf->map + elf->sec[e->e_shstrndx].sh_offset);
	}

	if (!!elf->sec) {
		foreach_sec(elf, sec) {
			switch (sec->sh_type) {
				case SHT_DYNSYM:
				case SHT_SYMTAB: 
					symtab_t stab = {
						.sec	= sec,
						.sym	= elf->map + sec->sh_offset,
						.str	= elf->map + elf->sec[sec->sh_link].sh_offset,
					};
					ll_add(&elf->ll_sym, &stab);
					break;
				default:
			}
		}
	}
	
	return 0;
}

// 64-bit Little Endian, PIE
__s8 check_elf(elf_t *elf) {
	Elf64_Ehdr *e = elf->ehdr;
	if (!e) return -1;

	if (!(((__u8*)e)[0] == 0x7f && ((__u8*)e)[1] == 'E' && ((__u8*)e)[2] == 'L' && ((__u8*)e)[3] == 'F'))
		return -1;

	if (e->e_ident[EI_CLASS] != ELFCLASS64) return -1;
	if (e->e_ident[EI_DATA]	!= ELFDATA2LSB) return -1;
	if (e->e_type != ET_DYN) {
		pr(PR_ERR "The ELF is not Position Independent / (ET_DYN)");
		return -1;
	}
	
	return 0;
}

__u64 elf_ftov(elf_t *elf, __u64 off) {
	if (!elf) return -1;
	foreach_phdr(elf, p) {
		if (p->p_offset <= off & p->p_offset + p->p_filesz > off)
			return off - p->p_offset + p->p_vaddr;
	}
	return 0;
}

__u64 elf_vtof(elf_t *elf, __u64 virt) {
	if (!elf) return -1;
	foreach_phdr(elf, p) {
		if (p->p_vaddr <= virt & p->p_vaddr + p->p_memsz > virt)
			return virt - p->p_vaddr + p->p_offset;
	}
	return 0;
}
