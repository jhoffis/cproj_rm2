#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifdef TEST_MODE
// Macro for asserting conditions
#define ASSERT(test, message) \
    do { \
        if (!(test)) { \
            fprintf(stderr, "[FAILED] %s at line %d in %s: %s\n", __func__, __LINE__, __FILE__, message); \
            exit(1); \
        } else { \
            fprintf(stdout, "[PASSED] %s at line %d in %s: %s\n", __func__, __LINE__, __FILE__, message); \
        } \
    } while (0)

// Macro for defining test cases
#define TEST(name) void name(void); __attribute__((constructor)) void register_##name(void) { name(); } void name(void)

#else

// Disable macros when TEST_MODE is not defined
#define ASSERT(test, message)
#define TEST(name) static inline void name(void)

#endif
