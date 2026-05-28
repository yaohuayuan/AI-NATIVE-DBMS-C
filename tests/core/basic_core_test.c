#include <stdio.h>
#include <string.h>

#include "aidb/aidb.h"

#define AIDB_ASSERT_TRUE(expr)                                \
    do {                                                      \
        if (!(expr)) {                                        \
            fprintf(stderr, "Assertion failed: %s\n", #expr); \
            return 1;                                         \
        }                                                     \
    } while (0)

int main(void)
{
    struct aidb_context context;

    AIDB_ASSERT_TRUE(aidb_status_is_ok(AIDB_OK));
    AIDB_ASSERT_TRUE(aidb_status_is_error(AIDB_ERROR_INTERNAL));
    AIDB_ASSERT_TRUE(strcmp(aidb_status_name(AIDB_OK), "AIDB_OK") == 0);
    AIDB_ASSERT_TRUE(strcmp(aidb_status_message(AIDB_ERROR_INVALID_ARGUMENT), "invalid argument") == 0);

    AIDB_ASSERT_TRUE(aidb_context_init(&context) == AIDB_OK);
    AIDB_ASSERT_TRUE(aidb_context_last_status(&context) == AIDB_OK);
    AIDB_ASSERT_TRUE(strcmp(aidb_context_error_message(&context), "") == 0);

    AIDB_ASSERT_TRUE(
        aidb_context_set_error(
            &context,
            AIDB_ERROR_INVALID_ARGUMENT,
            "invalid input"
        ) == AIDB_OK
    );

    AIDB_ASSERT_TRUE(aidb_context_last_status(&context) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_ASSERT_TRUE(strcmp(aidb_context_error_message(&context), "invalid input") == 0);

    aidb_context_clear_error(&context);
    AIDB_ASSERT_TRUE(aidb_context_last_status(&context) == AIDB_OK);
    AIDB_ASSERT_TRUE(strcmp(aidb_context_error_message(&context), "") == 0);

    aidb_context_deinit(&context);

    printf("basic_core_test passed.\n");
    return 0;
}