#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <aidb/error.h>
#include <aidb/rbt.h>
#include <support/test_assert.h>

static int compare_int(const void *left, const void *right)
{
    const int left_value = *(const int *)left;
    const int right_value = *(const int *)right;

    return (left_value > right_value) - (left_value < right_value);
}

static uint32_t next_random(uint32_t *state)
{
    *state = (*state * UINT32_C(1664525)) + UINT32_C(1013904223);
    return *state;
}

static int insert_values(aidb_rbt *tree, int *values, size_t count)
{
    size_t index;

    for (index = 0; index < count; ++index) {
        bool inserted = false;

        AIDB_TEST_ASSERT_TRUE(
            aidb_rbt_insert(tree, &values[index], &inserted) == AIDB_OK
        );
        AIDB_TEST_ASSERT_TRUE(inserted);
        AIDB_TEST_ASSERT_TRUE(tree->size == index + 1);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(tree));
    }

    return 0;
}

static int test_rbt_init_and_arguments(void)
{
    aidb_rbt tree;
    bool inserted = true;
    bool removed = true;
    void *removed_payload = (void *)&tree;
    int value = 10;

    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_init(NULL, compare_int) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_init(&tree, NULL) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));

    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(NULL, &value, &inserted) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(&tree, NULL, &inserted) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(&tree, &value, NULL) == AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(NULL, &value) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, NULL) == NULL);
    AIDB_TEST_ASSERT_TRUE(!aidb_rbt_validate(NULL));

    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_remove(NULL, &value, &removed_payload, &removed) ==
        AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_remove(&tree, NULL, &removed_payload, &removed) ==
        AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_remove(&tree, &value, NULL, &removed) ==
        AIDB_ERROR_INVALID_ARGUMENT
    );
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_remove(&tree, &value, &removed_payload, NULL) ==
        AIDB_ERROR_INVALID_ARGUMENT
    );

    aidb_rbt_deinit(&tree);
    AIDB_TEST_ASSERT_TRUE(tree.root == NULL);
    AIDB_TEST_ASSERT_TRUE(tree.nil == NULL);
    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    AIDB_TEST_ASSERT_TRUE(tree.compare == NULL);
    aidb_rbt_deinit(&tree);
    aidb_rbt_deinit(NULL);

    return 0;
}

static int test_rbt_insert_find_duplicate(void)
{
    aidb_rbt tree;
    int values[] = {10, 20, 30, 15, 25, 5, 1, 6, 14, 16};
    const size_t count = sizeof(values) / sizeof(values[0]);
    size_t index;
    int duplicate = 15;
    int missing = 99;
    bool inserted = true;

    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(insert_values(&tree, values, count) == 0);

    for (index = 0; index < count; ++index) {
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &values[index]) == &values[index]);
    }
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &missing) == NULL);

    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_insert(&tree, &duplicate, &inserted) == AIDB_OK
    );
    AIDB_TEST_ASSERT_TRUE(!inserted);
    AIDB_TEST_ASSERT_TRUE(tree.size == count);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &duplicate) == &values[3]);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));

    aidb_rbt_deinit(&tree);
    return 0;
}

static int test_rbt_remove_shapes(void)
{
    aidb_rbt tree;
    int values[] = {20, 10, 30, 5, 15, 25, 40, 13, 17, 27};
    const size_t count = sizeof(values) / sizeof(values[0]);
    int removal_order[] = {5, 25, 30, 20, 10, 13, 15, 17, 27, 40};
    const size_t removal_count =
        sizeof(removal_order) / sizeof(removal_order[0]);
    size_t index;
    int missing = 999;
    bool removed;
    void *removed_payload;

    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(insert_values(&tree, values, count) == 0);

    removed = true;
    removed_payload = &tree;
    AIDB_TEST_ASSERT_TRUE(
        aidb_rbt_remove(&tree, &missing, &removed_payload, &removed) == AIDB_OK
    );
    AIDB_TEST_ASSERT_TRUE(!removed);
    AIDB_TEST_ASSERT_TRUE(removed_payload == NULL);
    AIDB_TEST_ASSERT_TRUE(tree.size == count);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));

    for (index = 0; index < removal_count; ++index) {
        int *stored = aidb_rbt_find(&tree, &removal_order[index]);

        AIDB_TEST_ASSERT_TRUE(stored != NULL);
        removed = false;
        removed_payload = NULL;
        AIDB_TEST_ASSERT_TRUE(
            aidb_rbt_remove(
                &tree,
                &removal_order[index],
                &removed_payload,
                &removed
            ) == AIDB_OK
        );
        AIDB_TEST_ASSERT_TRUE(removed);
        AIDB_TEST_ASSERT_TRUE(removed_payload == stored);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_find(&tree, &removal_order[index]) == NULL);
        AIDB_TEST_ASSERT_TRUE(tree.size == removal_count - index - 1);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));
    }

    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));
    aidb_rbt_deinit(&tree);
    return 0;
}

static int test_rbt_sorted_stress(void)
{
    enum { VALUE_COUNT = 128 };
    aidb_rbt tree;
    int values[VALUE_COUNT];
    size_t index;

    for (index = 0; index < VALUE_COUNT; ++index) {
        values[index] = (int)index;
    }

    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(insert_values(&tree, values, VALUE_COUNT) == 0);

    for (index = 1; index < VALUE_COUNT; index += 2) {
        bool removed = false;
        void *removed_payload = NULL;

        AIDB_TEST_ASSERT_TRUE(
            aidb_rbt_remove(
                &tree,
                &values[index],
                &removed_payload,
                &removed
            ) == AIDB_OK
        );
        AIDB_TEST_ASSERT_TRUE(removed);
        AIDB_TEST_ASSERT_TRUE(removed_payload == &values[index]);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));
    }

    for (index = 0; index < VALUE_COUNT; index += 2) {
        bool removed = false;
        void *removed_payload = NULL;

        AIDB_TEST_ASSERT_TRUE(
            aidb_rbt_remove(
                &tree,
                &values[index],
                &removed_payload,
                &removed
            ) == AIDB_OK
        );
        AIDB_TEST_ASSERT_TRUE(removed);
        AIDB_TEST_ASSERT_TRUE(removed_payload == &values[index]);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));
    }

    AIDB_TEST_ASSERT_TRUE(tree.size == 0);
    aidb_rbt_deinit(&tree);
    return 0;
}

static int test_rbt_randomized_operations(void)
{
    enum {
        VALUE_COUNT = 257,
        OPERATION_COUNT = 10000
    };
    aidb_rbt tree;
    int values[VALUE_COUNT];
    bool present[VALUE_COUNT] = {false};
    uint32_t random_state = UINT32_C(0x5eed1234);
    size_t expected_size = 0;
    size_t index;

    for (index = 0; index < VALUE_COUNT; ++index) {
        values[index] = (int)index;
    }

    AIDB_TEST_ASSERT_TRUE(aidb_rbt_init(&tree, compare_int) == AIDB_OK);

    for (index = 0; index < OPERATION_COUNT; ++index) {
        const size_t value_index =
            (size_t)(next_random(&random_state) % VALUE_COUNT);
        const uint32_t operation = next_random(&random_state) % UINT32_C(3);

        if (operation == 0) {
            bool inserted = true;

            AIDB_TEST_ASSERT_TRUE(
                aidb_rbt_insert(&tree, &values[value_index], &inserted) ==
                AIDB_OK
            );
            AIDB_TEST_ASSERT_TRUE(inserted == !present[value_index]);
            if (inserted) {
                present[value_index] = true;
                ++expected_size;
            }
        } else if (operation == 1) {
            bool removed = true;
            void *removed_payload = &tree;

            AIDB_TEST_ASSERT_TRUE(
                aidb_rbt_remove(
                    &tree,
                    &values[value_index],
                    &removed_payload,
                    &removed
                ) == AIDB_OK
            );
            AIDB_TEST_ASSERT_TRUE(removed == present[value_index]);
            AIDB_TEST_ASSERT_TRUE(
                removed_payload ==
                (present[value_index] ? (void *)&values[value_index] : NULL)
            );
            if (removed) {
                present[value_index] = false;
                --expected_size;
            }
        } else {
            AIDB_TEST_ASSERT_TRUE(
                aidb_rbt_find(&tree, &values[value_index]) ==
                (present[value_index] ? (void *)&values[value_index] : NULL)
            );
        }

        AIDB_TEST_ASSERT_TRUE(tree.size == expected_size);
        AIDB_TEST_ASSERT_TRUE(aidb_rbt_validate(&tree));
    }

    for (index = 0; index < VALUE_COUNT; ++index) {
        AIDB_TEST_ASSERT_TRUE(
            aidb_rbt_find(&tree, &values[index]) ==
            (present[index] ? (void *)&values[index] : NULL)
        );
    }

    aidb_rbt_deinit(&tree);
    return 0;
}

int main(void)
{
    AIDB_TEST_ASSERT_TRUE(test_rbt_init_and_arguments() == 0);
    AIDB_TEST_ASSERT_TRUE(test_rbt_insert_find_duplicate() == 0);
    AIDB_TEST_ASSERT_TRUE(test_rbt_remove_shapes() == 0);
    AIDB_TEST_ASSERT_TRUE(test_rbt_sorted_stress() == 0);
    AIDB_TEST_ASSERT_TRUE(test_rbt_randomized_operations() == 0);

    return 0;
}
