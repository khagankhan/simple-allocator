#include "error_handling.h"
#include "mm.h"
#include "memlib.h"

#define FIRST_REQUEST 13
#define SECOND_REQUEST 6

int main(void) {
    mem_init();
    if(mm_init() == -1) {
        exit_on_error("mm_init() failed\n");
    }
    char* a = mm_malloc(FIRST_REQUEST);
    printf("a is allocated at: %p\n", a);
    for(int i = 0; i < FIRST_REQUEST; i++) {
        a[i] = i;
    }
    printf("Size of a block: %d\n", GET_SIZE(HDRP(a)));
    for(int i = 0; i < FIRST_REQUEST; i++) {
        printf("a[%d] = %d;\n", i, a[i]);
    }
    printf("a is freed\n");
    mm_free(a);
    printf("Size of a block: %d\n", GET_SIZE(HDRP(a)));
    char* b = mm_malloc(SECOND_REQUEST);
    printf("Size of a block: %d\n", GET_SIZE(HDRP(b)));
    for(int i = 0; i < SECOND_REQUEST; i++) {
        b[i] = i * 10;
    }
    for(int i = 0; i < FIRST_REQUEST; i++) {
        printf("b[%d] = %d;\n", i, a[i]);
    }
    return 0;
}