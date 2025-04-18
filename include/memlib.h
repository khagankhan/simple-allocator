#include "error_handling.h"

static char* mem_heap;
static char* mem_brk;
static char* mem_max_addr;

#define MAX_HEAP (20 * (1 << 20))  // 20 MB

void mem_init(void);
void* mem_sbrk(int incr);
