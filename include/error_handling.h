#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef DEBUG
  #define DEBUG_PRINT(...)            \
    do {                              \
      fprintf(stderr, "[*] DEBUG: "); \
      fprintf(stderr, __VA_ARGS__);   \
    } while(0)
#else
  #define DEBUG_PRINT(...)
#endif

#ifdef DEBUG
  #define DEBUG_INFO()
#else
  #define DEBUG_INFO() fprintf(stderr, "Compile with cmake .. -DDEBUG=ON for more info\n")
#endif

#define exit_on_error(...)                          \
    do {                                            \
        fprintf(stderr, "[-] Error: ");             \
        fprintf(stderr, __VA_ARGS__);               \
        fprintf(stderr, "\n");                      \
        DEBUG_INFO();                               \
        exit(1);                                    \
    } while (0)

    