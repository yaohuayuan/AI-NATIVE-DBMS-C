#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <aidb/error.h>
#include <aidb/rbt.h>
#include <support/test_assert.h>

static size_t allocation_count;
static size_t failing_allocation;

void *aidb_malloc(size_t size)
{
    ++allocation_count;
    if (allocation_count == failing_allocation) {
        return NULL;
    }
    return malloc(size);
}

void aidb_free(void *pointer)
{
    free(pointer);
}

static int compare_int(const void *left, const void *right)
{
    const int left_value = *(const int *)left;
    const int right_value = *(const int *)right;

    return (left_value > right_value) - (left_value < right_value);
}

static int test_init_allocation_failure(void)
{
    aidb_rbt tree;

    allocation_count = 0;
    failing_allocation = 1;

    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_init(&tree, compare_int) == AIDB_ERROR_OUT_OF_MEMORY
    );
    AIDB_TEST_ASSERT_TRUE(tree.root == NULL);
    AIDB_TEST_ASSERT_TRUE(tree.nil == NULL);
    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    AIDB_TEST_ASSERT_TRUE(!aidb_rbt_validate(&tree));

    aidb_rbt_deinit(&tree);
    return 0;
}

static int test_insert_allocation_failure(void)
{
    aidb_rbt tree;
    int value = 42;
    bool inserted = true;

    allocation_count = 0;
    failing_allocation = 2;

    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(&tree, &value, &inserted) == AIDB_ERROR_OUT_OF_MEMORY
    );
    AIDB_TEST_ASSERT_TRUE(!inserted);
    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &value) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));

    failing_allocation = 0;
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(&tree, &value, &inserted) == AIDB_OK
    );
    AIDB_TEST_ASSERT_TRUE(inserted);
    AIDB_TEST_ASSERT_TRUE(tree.size == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &value) == &value);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));

    aidb_rbt_deinit(&tree);
    return 0;
}

int main(void)
{
    AIDB_TEST_ASSERT_TRUE(test_init_allocation_failure() == 0);
    AIDB_TEST_ASSERT_TRUE(test_insert_allocation_failure() == 0);

    return 0;
}
