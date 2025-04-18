#include "mm.h"
#include "memlib.h"

static char* heap_listp = 0; 

// Forward declarations of the private functions
static void *extend_heap(size_t words);
static void* coalesce(void* bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

int mm_init(void) {
    // Request the initial 16 bytes of heap space (4 words * 4 bytes each = 16 bytes)
    // Layout will be:
    // [0] Padding (4 bytes) to ensure 8-byte alignment
    // [1] Prologue Header (4 bytes) — size = 8, allocated = 1 → 0x9
    // [2] Prologue Footer (4 bytes) — same as header
    // [3] Epilogue Header (4 bytes) — size = 0, allocated = 1 → 0x1
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1) {
        return -1;  // Allocation failed
    }
    // [0] Padding word for alignment — never used, just ensures heap is 8-byte aligned
    PUT(heap_listp, 0);
    // [1] Prologue header — a fake allocated block of size 8 bytes
    // Prevents edge cases when coalescing with the first real free block
    PUT(heap_listp + (1 * WSIZE), SET_ALLOCATION_BIT(DSIZE, 1));
    // [2] Prologue footer — same as header (makes it a valid block)
    PUT(heap_listp + (2 * WSIZE), SET_ALLOCATION_BIT(DSIZE, 1));
    // [3] Epilogue header — special zero-size allocated block that always follows the last block
    // Prevents edge cases when coalescing at the end of the heap
    PUT(heap_listp + (3 * WSIZE), SET_ALLOCATION_BIT(0, 1));
    // Move heap_listp to point to the payload of the prologue block (between header and footer)
    // This makes heap_listp behave like a "block pointer" (bp) in all other places
    heap_listp += (2 * WSIZE);
    // Extend the heap with an initial free block of PAGE_SIZE bytes
    // The number of words = PAGE_SIZE / 4 (WSIZE)
    if (extend_heap(PAGE_SIZE / WSIZE) == NULL) {
        return -1;
    }
    // Heap successfully initialized
    return 0;
}
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), SET_ALLOCATION_BIT(size, 0));
    PUT(FTRP(bp), SET_ALLOCATION_BIT(size,0));
    coalesce(bp);
}
void *mm_malloc(size_t size) {
    size_t asize;
    size_t extendsize;
    char* bp;
    if(size == 0) {
        return NULL;
    }
    if(size <= DSIZE) {
        asize = 2*DSIZE;
    } else {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }
    DEBUG_PRINT("Adjusted size in mm_malloc(%d) is %d\n", size, asize);
    if((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, PAGE_SIZE);
    if((bp = extend_heap(extendsize/WSIZE)) == NULL) {
        return NULL;
    } else {
        place(bp, asize);
        return bp;
    }
}
static void* coalesce(void* bp) {
    size_t prev_alloc = IS_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = IS_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc) {
        return bp;
    } else if(prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), SET_ALLOCATION_BIT(size, 0));
        PUT(FTRP(bp), SET_ALLOCATION_BIT(size, 0));
    } else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), SET_ALLOCATION_BIT(size, 0));
        PUT(FTRP(bp), SET_ALLOCATION_BIT(size,0));
        bp = PREV_BLKP(bp);
    } else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), SET_ALLOCATION_BIT(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), SET_ALLOCATION_BIT(size,0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
static void *extend_heap(size_t word_size) {
    char *bp;      // Block pointer (will point to the payload of new free block)
    size_t size;
    // Ensure alignment: if word_size is odd, round it up to even
    // because block size must be a multiple of 8 (for 8-byte alignment)
    size = (word_size % 2) ? (word_size + 1) * WSIZE : word_size * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) {
        return NULL; // Failed to grow heap
    }
    // Initialize new free block: write its header and footer
    PUT(HDRP(bp), SET_ALLOCATION_BIT(size, 0));      // New free block header. This overwrites the old epilogue
    PUT(FTRP(bp), SET_ALLOCATION_BIT(size, 0));      // New free block footer
    PUT(HDRP(NEXT_BLKP(bp)), SET_ALLOCATION_BIT(0, 1)); // New epilogue since old is overwritten
    // Since old epilogue becomes new header we can find the next block...
    // So since next_blkp is just #define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
    // It gives us the pointer to the last word size.
    // In this case we set it as new epilogue.
    /*
    After extend_heap:
    +---------------------------+
    | ... existing blocks ...   |
    +---------------------------+
    | New Free Block Header     | <- bp (payload starts right after)
    | Payload (unused space)    |
    | New Free Block Footer     |
    +---------------------------+
    | New Epilogue Header (0/1) | <- brk (heap top now here)
    +---------------------------+
    */
    return coalesce(bp);
}
static void *find_fit(size_t asize) {
    void* bp;
    for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!IS_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;
}
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    size_t remainder = csize - asize;
    if(remainder >= (2*DSIZE)) {
        PUT(HDRP(bp), SET_ALLOCATION_BIT(asize,1));
        PUT(FTRP(bp), SET_ALLOCATION_BIT(asize,1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), SET_ALLOCATION_BIT(remainder,0));
        PUT(FTRP(bp), SET_ALLOCATION_BIT(remainder,0));
    } else {
        PUT(HDRP(bp), SET_ALLOCATION_BIT(csize,1));
        PUT(FTRP(bp), SET_ALLOCATION_BIT(csize,1));
    }
}