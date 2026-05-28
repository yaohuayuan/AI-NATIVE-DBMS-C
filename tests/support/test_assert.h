#ifndef AIDB_TEST_ASSERT_H
#define AIDB_TEST_ASSERT_H

#include <stdio.h>

#define AIDB_TEST_ASSERT_TRUE(expr)                                      \
    do {                                                                \
        if (!(expr)) {                                                  \
            fprintf(stderr, "%s:%d: Assertion failed: %s\n",            \
                    __FILE__, __LINE__, #expr);                         \
            return 1;                                                   \
        }                                                               \
    } while (0)

#endif
