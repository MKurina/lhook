#include <linux/types.h>

typedef struct {
	#pragma pack(1)
	struct {
		__u8	*mem;
		__u64	size;
		__u64	curr;
	} in_map;

	union {
		__u8 *tbl_map;
		struct {
			struct {
				__u8 *lib, *fn;
			} str_info;
			void	*src;
			void	*hook;
			__u8	do_ret;

			__u8	active;
			__u64	in_off, in_sz;
		} *tbl;
	};
	__u64 tbl_size;
	__u64 hook_num;
	#pragma pack()
} stub_table_t;

void NIX_init();
void NIX_register(__u8 *lib, __u8 *fn, void *dst_fn, __u8 shall_return);
void *NIX_orig_fn(void *addr);