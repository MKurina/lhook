#include <linux/types.h>
#include <elf.h>

#define ll_type(T)					\
		struct LL##T {				\
			__u8 used;				\
			struct LL##T *next, *prev;\
			T data;					\
		}							\

#define ll_each(ll, _e)				\
		for (typeof((ll)) _e = (ll); !!_e && !!_e->used; _e = _e->next)

#define ll_rev(ll, e)				\
		for (typeof((ll)) e = (ll)->prev; !!e; e = e->prev)

#define ll_each_val(ll, v)			\
		ll_each((ll), _e_) for (typeof(&((ll)->data)) v = &(_e_)->data ; !!v; v=NULL)

#define ll_free_all(ll)				\
		ll_rev(&elf.ll_sym, e) if (e != &elf.ll_sym) free(e); else memset(e, 0, sizeof(*e));

// ll, v shall be pointers
#define ll_add(ll, v)	({						\
		if (!(ll)->used) {						\
			_ll_add_new_((ll), v);				\
		} else {								\
			ll_each((ll), _e_) if (!_e_->next) {\
				_e_->next = xmalloc(sizeof(typeof((*_e_))));	\
				_e_->next->prev = _e_;			\
				(ll)->prev = _e_->next;			\
				_ll_add_new_((_e_->next), v);	\
				break;							\
			}									\
		}										\
	})

#define _ll_add_new_(x, _v)	{					\
		(x)->used = 1;							\
		memcpy(&((x)->data), (_v), sizeof(typeof(((x)->data))));	\
	}


typedef struct {
	Elf64_Shdr	*sec;
	Elf64_Sym	*sym;
	__u8		*str;
} symtab_t;

typedef struct {
	union {
		void		*map;
		Elf64_Ehdr *ehdr;
	};
	__u64 size;

	Elf64_Phdr *phdr;
	__u16		ph_num;

	Elf64_Shdr *sec;
	__u16		sec_num;
	__u8		*sec_strtab;

	ll_type(symtab_t)	 ll_sym;
} elf_t;

typedef void *(*ifunc_t)();

#define foreach_phdr(elf, p)			\
		for (Elf64_Phdr *p = (elf)->phdr; p < &(elf)->phdr[(elf)->ph_num]; p++)

#define foreach_sec(elf, s)				\
		for (Elf64_Shdr *s = (elf)->sec; s < &(elf)->sec[(elf)->sec_num]; s++)

#define foreach_sym(sym, stab)			\
		for (Elf64_Sym *sym = (stab)->sym; sym < (void*)(stab)->sym + (stab)->sec->sh_size; sym++)

__s8 check_elf(elf_t *elf);
__s8 load_mem_elf(elf_t *elf, void *addr, __u64 size);
__u64 elf_ftov(elf_t *elf, __u64 off);
__u64 elf_vtof(elf_t *elf, __u64 virt);