#include <linux/types.h>
#include <link.h>
#include <sys/mman.h>
#include "../utils/utils.h"
#include "../include/elf.h"
#include "../include/hooks.h"
#ifndef UAPI_H
#define UAPI_H
#include "../disas-x86_64/include/uapi.h"
#endif

static int _wr_init(void *addr, __u16 page_num);
static int _wr_exit(void *addr, __u16 page_num, int prot);
static stub_table_t table = {0};
static __u8 trampoline_sc[10] =	"\x68\xef\xbe\xad\xde"	// push 0
								"\xe9\xef\xbe\xad\xde";	// jmp 0

void NIX_register(__u8 *lib, __u8 *fn, void *dst_fn, __u8 ret) {
	// Resize
	if (sizeof(*table.tbl) * table.hook_num +1 > table.tbl_size) {
		table.tbl_map	= map_replace(table.tbl_map, PROT_RW, table.tbl_size, 0x1000);
		table.tbl_size	+= 0x1000;
	}
	table.tbl[table.hook_num++] = (typeof(*table.tbl)){
		.str_info = { .lib=lib, fn=fn },
		.hook	= dst_fn,
		.do_ret	= !!ret,
	};
}

__s64 hook_fix_orig(void *stub, void *addr) {
	__u8 *ptr = &table.in_map.mem[table.in_map.curr];
	__u64 src=addr, dst=stub;

	__u64 i = 0;
	for (; i < sizeof(trampoline_sc);) {
		instr_dat_t in = {0};
		if (init_instr(addr + i, &in) == -1) return -1;

		__u64 imm = 0;
		get_rip_ptr_addr(&in, src, &imm);
		if (!!imm) {
			if (change_ptr(&in, (dst - src) - (imm - src) - in.in_sz, CHNG_REL) == -1)
				return -1;
		}
		__u8 ret = assemble(&in, ptr + i);
		_assert(ret == in.in_sz, "FUCK!!!");

		i += in.in_sz;
	}

	*(ptr+i) = 0xe9;
	*(__u32*)(ptr+i+1) = (src + i) - ((__u64)ptr+i) - 5;

	return i + 5;
}
#define each_hook(table, e)				\
		for (typeof((table)->tbl) e = (table)->tbl; e < &(table)->tbl[(table)->hook_num]; e++)

void *NIX_orig_fn(void *addr) {
	__u64 i = 0;
	each_hook(&table, h) {
		if (h->src == addr)
			return table.in_map.mem + h->in_off;
	}
	return addr;
}

extern void stub();
void hook_api_init() {
	table.tbl_map	= map_anon(0x1000);
	table.tbl_size	= 0x1000;

	table.in_map.mem	= map_anon_exec(0x1000);
	table.in_map.size	= 0x1000;

	NIX_init();
}

__attribute__((constructor())) void init() {
	hook_api_init();
	typeof(&table.in_map) in_map = &table.in_map;

	__u64 trmp_sz = sizeof(trampoline_sc);
	struct link_map *start = _r_debug.r_map;
	while (start) {
		elf_t elf = {0};
		__u8 *l_name = start->l_name;
		if (!*l_name) l_name = "/proc/self/exe";
		if (l_name[0] != '/') goto next;

		each_hook(&table, e) if (!e->active) {
			if (!strncmp(l_name, e->str_info.lib, strlen(l_name)+1))
				goto do_hook;
		}

		goto next;
		do_hook:


		if (load_elf(&elf, l_name) == -1)
			return -1;

		ll_each_val(&elf.ll_sym, e) {
			foreach_sym(sym, e) {
				if (sym->st_size && (sym->st_info & STT_FUNC)) {
					if (sym->st_size < sizeof(trampoline_sc))
						continue;

					__u8 *sym_name = &e->str[sym->st_name];
					__u64 sym_virt = start->l_addr + elf_ftov(&elf, sym->st_value);

					if (ELF64_ST_TYPE(sym->st_info) == STT_GNU_IFUNC)
						sym_virt = ((ifunc_t)sym_virt)();

					each_hook(&table, h) {
						if (!!h->active) continue;
						if (	!strncmp(sym_name,	h->str_info.fn, strlen(sym_name)+1)
							&&	!strncmp(l_name,	h->str_info.lib, strlen(l_name)+1))
						{
							/* Copy orig instr */
							__s64 b = hook_fix_orig(stub, sym_virt);
							if (b == -1) return -1;
							h->in_off		= in_map->curr;
							in_map->curr	+= b;

							// Resize
							if (in_map->curr + b + 0x100 > in_map->size) {
								in_map->mem = map_replace(in_map->mem, PROT_RWE, in_map->size, 0x1000);
								in_map->size += 0x1000;
								if (!in_map->mem) return -1;
							}

							/** Insert hook **/
							__u8 *ptr = sym_virt;
							_wr_init(sym_virt, 2);

							memcpy(ptr, trampoline_sc, sizeof(trampoline_sc));
							*((__u32*)&ptr[1]) = (h - table.tbl);
							*((__u32*)&ptr[6]) = ((void*)stub - (sym_virt + sizeof(trampoline_sc)));

							_wr_exit(sym_virt, 2, PROT_RE);
							/* Insert &table into the stub */
							_wr_init(stub, 1);
							*(__u64*)(&((__u8*)stub)[0xe]) = &table;
							_wr_exit(stub, 1, PROT_RE);

							h->src		= sym_virt;
							h->active	= 1;
						}
					}
				}
			}
		}

		munmap(elf.map, elf.size);
		ll_free_all(&elf.ll_sym);
		next:
			start = start->l_next;
	}
	return 0;
}

static int _wr_init(void *addr, __u16 page_num) {
	return mprotect((__u64)addr & ~0xfff, PAGE_SZ*page_num, PROT_READ | PROT_WRITE | PROT_WRITE);
}

static int _wr_exit(void *addr, __u16 page_num, int prot) {
	return mprotect((__u64)addr & ~0xfff, PAGE_SZ*page_num, prot & ~PROT_WRITE);
}
