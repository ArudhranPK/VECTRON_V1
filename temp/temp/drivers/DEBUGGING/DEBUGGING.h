#ifndef SRC_DRIVERS_DEBUGGING_DEBUGGING_H_
#define SRC_DRIVERS_DEBUGGING_DEBUGGING_H_

#include <stdio.h>

#define DEBUG_ENABLED 1

#if DEBUG_ENABLED == 1
// __VA_ARGS__ passes all arguments (strings + variables) to printf
#define debugging(...) printf(__VA_ARGS__)
#else
    // Compiles to nothing
    #define debugging(...) ((void)0)
#endif

int _write(int file, char *ptr, int len);

#endif
