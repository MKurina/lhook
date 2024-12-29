#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "../include/hooks.h"

#define SELF	"/proc/self/exe"
#define LIBC	"/lib/x86_64-linux-gnu/libc.so.6"

#define orig_fn(addr, ...) ({								\
		typeof((addr))(*__fn) = NIX_orig_fn((void*)(addr));	\
		(__fn(__VA_ARGS__));								\
})

static int fake_socket(int a, int b, int c);
static int fake_strcmp(__u8 *a, __u8 *b);
static int fake_getenv(__u8 *str);

void NIX_init() {
	NIX_register(LIBC, "socket", fake_socket, 1);
	NIX_register(LIBC, "strcmp", fake_strcmp, 1);
	NIX_register(LIBC, "getenv", fake_getenv, 1);
}

static int fake_socket(int a, int b, int c) {
	printf("FROM [socket] (%u %u %u)\n", a, b, c);
	return orig_fn(socket, a, b, c);
}

static int fake_strcmp(__u8 *a, __u8 *b) {
	printf("\n\t\tFROM [strcmp] : %s %s\n", a, b);
	return orig_fn(strcmp, a, b);
}

static int fake_getenv(__u8 *str) {
	printf("FROM [getenv] %-15s = %s\n", str, orig_fn(getenv, str));
	return orig_fn(getenv, str);
}