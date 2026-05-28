#include <stdio.h>
#include <string.h>

#include "aidb/aidb.h"

#define AIDB_ASSERT_TRUE(expr)                                      \
    do {                                                            \
        if (!(expr)) {                                              \
            fprintf(stderr, "Assertion failed: %s\n", #expr);       \
            return 1;                                               \
        }                                                           \
    } while (0)

int main(void) {
    AidbContext ctx;

    aidb_init(&ctx);

    AIDB_ASSERT_TRUE(ctx.initialized == 1);
    AIDB_ASSERT_TRUE(ctx.project_name != 0);
    AIDB_ASSERT_TRUE(strcmp(ctx.project_name, "AI-NATIVE-DBMS-C") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_version(), "0.1.0") == 0);

    aidb_shutdown(&ctx);

    AIDB_ASSERT_TRUE(ctx.initialized == 0);

    printf("basic_core_test passed.\n");

    return 0;
}
