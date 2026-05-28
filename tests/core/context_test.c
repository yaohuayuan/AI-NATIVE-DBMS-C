#include <stdio.h>
#include <string.h>

#include "aidb/context.h"

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

    AIDB_ASSERT_TRUE(aidb_context_init(NULL) == AIDB_ERROR_INVALID_ARGUMENT);

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

    AIDB_ASSERT_TRUE(aidb_context_set_error(&context, AIDB_ERROR_PARSE, NULL) == AIDB_OK);
    AIDB_ASSERT_TRUE(strcmp(aidb_context_error_message(&context), "parse error") == 0);

    AIDB_ASSERT_TRUE(
        aidb_context_set_error(
            NULL,
            AIDB_ERROR_INTERNAL,
            "x"
        ) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_ASSERT_TRUE(aidb_context_last_status(NULL) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_ASSERT_TRUE(aidb_context_error_message(NULL) != NULL);
    AIDB_ASSERT_TRUE(strcmp(aidb_context_error_message(NULL), "null context") == 0);

    aidb_context_clear_error(NULL);
    aidb_context_deinit(NULL);
    aidb_context_deinit(&context);

    printf("context_test passed.\n");
    return 0;
}
