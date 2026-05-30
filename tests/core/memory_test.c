#include <stdio.h>
#include <string.h>

#include "aidb/memory.h"
#include "support/test_assert.h"

int main(void)
{
    int *value = aidb_malloc(sizeof(int));
    AIDB_TEST_ASSERT_TRUE(value != NULL);
    *value = 42;
    AIDB_TEST_ASSERT_TRUE(*value == 42);
    aidb_free(value);

    aidb_free(NULL);

    int *items = aidb_calloc(4, sizeof(int));
    AIDB_TEST_ASSERT_TRUE(items != NULL);
    for (int i = 0; i < 4; ++i) {
        AIDB_TEST_ASSERT_TRUE(items[i] == 0);
    }
    aidb_free(items);

    char *text = aidb_malloc(8);
    AIDB_TEST_ASSERT_TRUE(text != NULL);
    strcpy(text, "aidb");

    char *expanded = aidb_realloc(text, 16);
    AIDB_TEST_ASSERT_TRUE(expanded != NULL);
    text = expanded;
    AIDB_TEST_ASSERT_TRUE(strcmp(text, "aidb") == 0);
    aidb_free(text);

    void *zero_size = aidb_malloc(0);
    aidb_free(zero_size);

    printf("memory_test passed.\n");
    return 0;
}
