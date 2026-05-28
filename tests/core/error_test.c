#include <stdio.h>
#include <string.h>

#include "aidb/error.h"

#define AIDB_ASSERT_TRUE(expr)                                \
    do {                                                      \
        if (!(expr)) {                                        \
            fprintf(stderr, "Assertion failed: %s\n", #expr); \
            return 1;                                         \
        }                                                     \
    } while (0)

int main(void)
{
    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_OK), "AIDB_OK") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_OK), "ok") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_INVALID_ARGUMENT), "AIDB_ERROR_INVALID_ARGUMENT") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_INVALID_ARGUMENT), "invalid argument") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_OUT_OF_MEMORY), "AIDB_ERROR_OUT_OF_MEMORY") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_OUT_OF_MEMORY), "out of memory") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_IO), "AIDB_ERROR_IO") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_IO), "io error") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_PARSE), "AIDB_ERROR_PARSE") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_PARSE), "parse error") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_NOT_FOUND), "AIDB_ERROR_NOT_FOUND") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_NOT_FOUND), "not found") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_ERROR_INTERNAL), "AIDB_ERROR_INTERNAL") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_INTERNAL), "internal error") == 0);

    AIDB_ASSERT_TRUE(strcmp(aidb_status_name((enum aidb_status)9999), "AIDB_ERROR_UNKNOWN") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message((enum aidb_status)9999), "unknown error") == 0);

    AIDB_ASSERT_TRUE(aidb_status_is_ok(AIDB_OK));
    AIDB_ASSERT_TRUE(!aidb_status_is_ok(AIDB_ERROR_INTERNAL));
    AIDB_ASSERT_TRUE(!aidb_status_is_error(AIDB_OK));
    AIDB_ASSERT_TRUE(aidb_status_is_error(AIDB_ERROR_INTERNAL));

    printf("error_test passed.\n");
    return 0;
}
