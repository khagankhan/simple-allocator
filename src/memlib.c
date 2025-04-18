/* Private global variables */
#include "memlib.h"

void mem_init(void) {
    mem_heap = (char*)malloc(MAX_HEAP);
    if(mem_heap == NULL) {
        exit_on_error("malloc returned NULL to mem_heap: %p", mem_heap);
    }
    mem_brk = (char*)mem_heap;
    mem_max_addr = (char*)(mem_heap + MAX_HEAP);
}
void* mem_sbrk(int incr) {
    char* old_brk = mem_brk;
    if((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
        fprintf(stderr, "[-] Error: mem_sbrk() failed.\n");
        DEBUG_PRINT("Error handling is propogated to the caller. ENOMEM is set to errno.\n");
        DEBUG_PRINT("[-] mem_sbrk: Request for %d bytes failed. Current heap: %ld bytes used.\n",
                incr, mem_brk - mem_heap);
        errno = ENOMEM;
        return (void*)-1; 
    }
    mem_brk += incr;
    return (void*)old_brk;
}
/*
Start of heap
|
+-----------------------+
|  Padding (unused)     |  <- 8 bytes, for alignment
+-----------------------+
|  Prologue Header: 8/1 |  <- size 8 bytes, allocated
+-----------------------+
|  Prologue Footer: 8/1 |
+-----------------------+
|  Block 1 Header: sz/a |  <- regular block begins
|  Payload / Free Space |
|  Block 1 Footer: sz/a |  <- only if free
+-----------------------+
|  Block 2 Header: sz/a |
|  Payload / Free Space |
|  Block 2 Footer: sz/a |
+-----------------------+
       ...
+-----------------------+
|  Block N Header: sz/a |
|  Payload / Free Space |
|  Block N Footer: sz/a |
+-----------------------+
| Epilogue Header: 0/1  |  <- always allocated
+-----------------------+

heap_listp → Points to Prologue Header

*/