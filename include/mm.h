#include <stddef.h>

#define WSIZE 4
#define DSIZE (2 * WSIZE)
#define PAGE_SIZE (4 * (1 << 10)) // 4KB

#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define SET_ALLOCATION_BIT(size, alloc_bit) ((size) | (alloc_bit))

#define GET(p) (*(unsigned int*)(p)) // Get the value at p. Cast it to unsigned int then dereference
#define PUT(p, val) (*(unsigned int*)(p) = (val)) // Write to the value to the address p. 
#define GET_SIZE(p) (GET(p) & ~0x7) // Get value from p, & it with 0b1000 (~0x7) to get size from header/footer.
#define IS_ALLOC(p) (GET(p) & 0x1) // If the alloc bit is set it is allocated from header/footer. Not free.
#define HDRP(bp) ((char*)(bp) - WSIZE) // Get the pointer to header
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // Get the pointer to footer
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE))) // Get the pointer to the next
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE))) // Get the pointer to the previous pointer

int mm_init(void);
void mm_free(void *bp);
void *mm_malloc(size_t size);
/*
Allocated or Free Block (Boundary Tag Format)
+----------------------+
|  Block Size | A/F    |  <- Header (4 bytes)
|  (29 bits)  |(3 bits)|
+----------------------+
|     Payload          |  <- Actual data area (size varies)
|     ...              |
|    (when allocated)  |
+----------------------+
|      Padding         |  <- Optional (for alignment, if needed)
+----------------------+
|  Block Size | A/F    |  <- Footer (4 bytes) — same format as header
|  (29 bits)  |(3 bits)|
+----------------------+
*/