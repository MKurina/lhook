#include <linux/types.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../include/helpers.h"

int (*prf)(const char *f, ...)	= printf;
int (*pr)(const char *f)		= puts;

void _die(__u8 *s) {
	prf("[-!-] FAILED - %s\n", s);
	prf("         - errno (%02x) - %s\n", errno, strerror(errno));
	pr("");
	exit(-1);
}

static __u8 *hexdump_char_color(__u8 c) {
	__u8 *clr = CRST;
	if (isprint(c)) {
		clr = BLUE;
	} else if (isspace(c) || !c) {
		clr = RED;
	} else if (c == 0xff) {
		clr = GRN;
	}
	return clr;
}

void hexdump(void *addr, __u64 sz) {
	__u8 *ptr = addr;

	for (__u64 i = 0; i <= sz; i++) {
		__u8 ch = ptr[i];

		if ( ( !(i % 0x10) && i) || i >= sz) {
			__u8 n = 0x10;

			if (i >= sz) {
				n = !(sz % 0x10) ? 0x10 : (sz % 0x10);
				for (int x = n; x < 0x10; x++) {
					if (!(i % 4)) prf("  ");
					prf("   ");
				}
			}

			prf("\t|\t");

			for (__u8 l = 0; l < n; l++) {
				__u8 c = ptr[ (i-n)+l ];
				__u8 *clr = hexdump_char_color(c);
				prf("%s%c" CRST, clr, isprint(c) ? c : '.');
			}

			pr("");
			if (i >= sz) return;
		}

		if (!i || !(i % 0x10))
			prf(CRST "0x%06x :\t", i);

		if ( !(i % 4) && i && (i % 0x10))
			prf("| ");

		__u8 *clr = hexdump_char_color(ptr[i]);
		prf("%s%02x " CRST, clr, ch);
	}
}

void *map_anon(__u64 sz) {
	void *map = mmap(0, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	memset(map, 0, sz);
	return map;
}

void *xmalloc(__u64 sz) {
	void *ret = malloc(sz);
	_assertf((!!ret), "Malloc got fucked! sz (0x%lx)", sz);
	memset(ret, 0, sz);
	return ret;
}

__s8 is_string(void *addr) {
	for (__u8 *ptr = addr; ptr < PAGE_ALIGN((__u64)addr); ptr++) {
		if (ptr != addr && *ptr == '\0') return 1;
		if (!isprint(*ptr)) return 0;
	}
	return 0;
}