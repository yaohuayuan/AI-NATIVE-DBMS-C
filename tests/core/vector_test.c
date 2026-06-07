#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "aidb/vector.h"
#include "aidb/error.h"
#include "support/test_assert.h"

struct vector_test_pair {
    int id;
    char name[16];
};

int main(void)
{
    aidb_vector vector;

    AIDB_TEST_ASSERT_TRUE(aidb_vector_init(NULL, sizeof(int)) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_vector_init(&vector, 0) == AIDB_ERROR_INVALID_ARGUMENT);

    AIDB_TEST_ASSERT_TRUE(aidb_vector_init(&vector, sizeof(int)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(vector.size == 0);
    AIDB_TEST_ASSERT_TRUE(vector.capacity == 0);
    AIDB_TEST_ASSERT_TRUE(vector.data == NULL);

    int value = 42;
    AIDB_TEST_ASSERT_TRUE(aidb_vector_push_back(&vector, &value) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(vector.size == 1);
    AIDB_TEST_ASSERT_TRUE(vector.capacity >= 1);

    int *first = aidb_vector_at(&vector, 0);
    AIDB_TEST_ASSERT_TRUE(first != NULL);
    AIDB_TEST_ASSERT_TRUE(*first == 42);

    for (int i = 0; i < 10; ++i) {
        int item = i + 100;
        AIDB_TEST_ASSERT_TRUE(aidb_vector_push_back(&vector, &item) == AIDB_OK);
        AIDB_TEST_ASSERT_TRUE(vector.size == (size_t)i + 2);
        AIDB_TEST_ASSERT_TRUE(vector.capacity >= vector.size);
    }

    AIDB_TEST_ASSERT_TRUE(*(int *)aidb_vector_at(&vector, 0) == 42);
    for (int i = 0; i < 10; ++i) {
        int *item = aidb_vector_at(&vector, (size_t)i + 1);
        AIDB_TEST_ASSERT_TRUE(item != NULL);
        AIDB_TEST_ASSERT_TRUE(*item == i + 100);
    }

    AIDB_TEST_ASSERT_TRUE(aidb_vector_at(&vector, vector.size) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_vector_at(NULL, 0) == NULL);

    const aidb_vector *const_vector = &vector;
    const int *const_first = aidb_vector_at_const(const_vector, 0);
    AIDB_TEST_ASSERT_TRUE(const_first != NULL);
    AIDB_TEST_ASSERT_TRUE(*const_first == 42);
    AIDB_TEST_ASSERT_TRUE(aidb_vector_at_const(const_vector, vector.size) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_vector_at_const(NULL, 0) == NULL);

    size_t old_capacity = vector.capacity;
    AIDB_TEST_ASSERT_TRUE(aidb_vector_reserve(&vector, old_capacity) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(vector.capacity >= old_capacity);

    AIDB_TEST_ASSERT_TRUE(aidb_vector_reserve(&vector, old_capacity + 20) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(vector.capacity >= old_capacity + 20);
    AIDB_TEST_ASSERT_TRUE(*(int *)aidb_vector_at(&vector, 0) == 42);
    for (int i = 0; i < 10; ++i) {
        int *item = aidb_vector_at(&vector, (size_t)i + 1);
        AIDB_TEST_ASSERT_TRUE(item != NULL);
        AIDB_TEST_ASSERT_TRUE(*item == i + 100);
    }

    aidb_vector_clear(&vector);
    AIDB_TEST_ASSERT_TRUE(vector.size == 0);
    AIDB_TEST_ASSERT_TRUE(vector.capacity >= old_capacity + 20);

    value = 77;
    AIDB_TEST_ASSERT_TRUE(aidb_vector_push_back(&vector, &value) == AIDB_OK);
    first = aidb_vector_at(&vector, 0);
    AIDB_TEST_ASSERT_TRUE(first != NULL);
    AIDB_TEST_ASSERT_TRUE(*first == 77);

    aidb_vector pair_vector;
    AIDB_TEST_ASSERT_TRUE(aidb_vector_init(&pair_vector, sizeof(struct vector_test_pair)) == AIDB_OK);

    struct vector_test_pair pair = {7, "seven"};
    AIDB_TEST_ASSERT_TRUE(aidb_vector_push_back(&pair_vector, &pair) == AIDB_OK);
    pair.id = 8;
    (void)strncpy(pair.name, "changed", sizeof(pair.name));
    pair.name[sizeof(pair.name) - 1] = '\0';

    struct vector_test_pair *stored_pair = aidb_vector_at(&pair_vector, 0);
    AIDB_TEST_ASSERT_TRUE(stored_pair != NULL);
    AIDB_TEST_ASSERT_TRUE(stored_pair->id == 7);
    AIDB_TEST_ASSERT_TRUE(strcmp(stored_pair->name, "seven") == 0);

    aidb_vector_deinit(&pair_vector);

    aidb_vector_deinit(&vector);
    AIDB_TEST_ASSERT_TRUE(vector.size == 0);
    AIDB_TEST_ASSERT_TRUE(vector.capacity == 0);
    AIDB_TEST_ASSERT_TRUE(vector.data == NULL);
    AIDB_TEST_ASSERT_TRUE(vector.element_size == 0);

    aidb_vector_deinit(NULL);

    return 0;
}
