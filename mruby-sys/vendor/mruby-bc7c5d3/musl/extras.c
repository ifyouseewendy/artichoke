#include <string.h>
#include <setjmp.h>

_Noreturn void abort() {
	__builtin_unreachable();
}

void exit(int status) {
	abort();
}

void free(void *ptr) {
	(void)ptr;
	/* Leak everything */
}

extern char __heap_base;
static char *HEAP = &__heap_base;

#define PAGE_SIZE 65536

void *realloc(void *ptr, size_t size) {
	while(((unsigned long)HEAP) % 8 != 0) HEAP++;
	void *newptr = HEAP;
	HEAP += size;
	char *mem_end = (char*)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
	if(mem_end < HEAP) {
		__builtin_wasm_memory_grow(0, ((HEAP - mem_end) / PAGE_SIZE) + 1);
	}
	if(ptr) {
		memmove(newptr, ptr, size);
	}
	return newptr;
}

void *malloc(size_t size) {
	return realloc(NULL, size);
}

// setjmp succeeds but does nothing
int setjmp(jmp_buf env) { return 0; }

// longjmp is a crash
void longjmp(jmp_buf env, int val) {
	abort();
}
