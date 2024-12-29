#include <linux/types.h>
#include <stdio.h>
#include <ctype.h>

#define BRED	"\e[1;31m"
#define RED		"\e[0;31m"
#define BGRN	"\e[1;32m"
#define GRN		"\e[0;32m"
#define BBLUE	"\e[1;34m"
#define BLUE	"\e[0;34m"
#define YLW		"\e[0;33m"
#define BYLW	"\e[1;33m"
#define CYAN	"\e[0;36m"
#define BCYAN	"\e[1;36m"
#define WHT		"\e[0;37m"
#define BWHT	"\e[1;37m"
#define CRST	"\e[0m"

#define color(c, str)	c str CRST

extern int (*prf)(const char *f, ...);
extern int (*pr)(const char *f);

#define PAGE_SZ	0x1000
#define PAGE_MASK (~(PAGE_SZ-1))
#define PAGE_ALIGN(x)		(((x) + PAGE_SZ-1) & PAGE_MASK)
#define _align(v, align)	(((v) + ((align) -1)) & ~((align)-1))
#define N_BITS_MAX(n)		((1ull << (n)) -1)

#define _dbg(fmt, ...)	prf("[+]  " fmt"\n", __VA_ARGS__);

#define _assert(b, s) if (!(b)) _die(s);
#define _assertf(b, s, ...) if (!(b)) {		\
			prf_x(PR_ERR, s, __VA_ARGS__);	\
			exit(-1);						\
		}

#define PR_ERR		BRED "  [-!-]  "CRST
#define PR_DBG		BGRN "  [+]    "CRST
#define PR_WARN		BYLW "  [WARN] "CRST
#define PR_INFO		BBLUE"  [info] "CRST

#define prf_x(t, str, ...)			\
		prf(t "@%s:%lu " str "\n", __FUNCTION__, __LINE__, __VA_ARGS__);

#define foreach(arr, e, type)		\
		for (type *e = &arr[0]; e < &arr[sizeof(arr)/sizeof(type)]; e++)

#define _zero(ptr, sz) { memset(ptr, 0, sz); }

#define DIE(str)		{ perror(str); exit(-1); }
#define FDIE(str, ...)	{ printf(str "\n", __VA_ARGS__); exit(-1); }

void hexdump(void *addr, __u64 sz);
void _die(__u8 *s);
void *map_anon(__u64 sz);
void *xmalloc(__u64 sz);
__s8 is_string(void *addr);