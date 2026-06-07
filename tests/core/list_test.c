#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "aidb/list.h"
#include "aidb/error.h"
#include "support/test_assert.h"

struct list_test_pair {
    int id;
    char name[16];
};

static int test_list_init(void)
{
    aidb_list list;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(NULL, sizeof(int)) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, 0) == AIDB_ERROR_INVALID_ARGUMENT);

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(aidb_list_size(&list) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(&list));
    AIDB_TEST_ASSERT_TRUE(aidb_list_front(&list) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_list_back(&list) == NULL);

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_push_back_order(void)
{
    aidb_list list;
    int value;
    int out;
    int *front;
    int *back;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);

    value = 1;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 2;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 3;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);

    AIDB_TEST_ASSERT_TRUE(aidb_list_size(&list) == 3);
    front = aidb_list_front(&list);
    back = aidb_list_back(&list);
    AIDB_TEST_ASSERT_TRUE(front != NULL);
    AIDB_TEST_ASSERT_TRUE(back != NULL);
    AIDB_TEST_ASSERT_TRUE(*front == 1);
    AIDB_TEST_ASSERT_TRUE(*back == 3);

    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 2);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 3);

    AIDB_TEST_ASSERT_TRUE(aidb_list_size(&list) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(&list));

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_push_front_order(void)
{
    aidb_list list;
    int value;
    int out;
    int *front;
    int *back;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);

    value = 1;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_front(&list, &value) == AIDB_OK);
    value = 2;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_front(&list, &value) == AIDB_OK);
    value = 3;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_front(&list, &value) == AIDB_OK);

    front = aidb_list_front(&list);
    back = aidb_list_back(&list);
    AIDB_TEST_ASSERT_TRUE(front != NULL);
    AIDB_TEST_ASSERT_TRUE(back != NULL);
    AIDB_TEST_ASSERT_TRUE(*front == 3);
    AIDB_TEST_ASSERT_TRUE(*back == 1);

    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 3);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 2);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(&list));

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_pop_back_order(void)
{
    aidb_list list;
    int value;
    int out;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);

    value = 10;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 20;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 30;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);

    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 30);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 20);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(&list, &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out == 10);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(&list));

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_empty_pop(void)
{
    aidb_list list;
    int out;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, &out) == AIDB_ERROR_NOT_FOUND);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(&list, &out) == AIDB_ERROR_NOT_FOUND);

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_front_back_const(void)
{
    aidb_list list;
    const aidb_list *const_list;
    const int *front;
    const int *back;
    int value = 42;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);

    const_list = &list;
    front = aidb_list_front_const(const_list);
    back = aidb_list_back_const(const_list);
    AIDB_TEST_ASSERT_TRUE(front != NULL);
    AIDB_TEST_ASSERT_TRUE(back != NULL);
    AIDB_TEST_ASSERT_TRUE(*front == 42);
    AIDB_TEST_ASSERT_TRUE(*back == 42);

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_clear(void)
{
    aidb_list list;
    int value;
    int *front;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);

    value = 1;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 2;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    value = 3;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);

    aidb_list_clear(&list);
    AIDB_TEST_ASSERT_TRUE(aidb_list_size(&list) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(&list));
    AIDB_TEST_ASSERT_TRUE(aidb_list_front(&list) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_list_back(&list) == NULL);

    value = 99;
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);
    front = aidb_list_front(&list);
    AIDB_TEST_ASSERT_TRUE(front != NULL);
    AIDB_TEST_ASSERT_TRUE(*front == 99);

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_struct_value_copy(void)
{
    aidb_list pair_list;
    struct list_test_pair pair = {7, "seven"};
    struct list_test_pair *stored_pair;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&pair_list, sizeof(struct list_test_pair)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&pair_list, &pair) == AIDB_OK);

    pair.id = 8;
    (void)strncpy(pair.name, "changed", sizeof(pair.name));
    pair.name[sizeof(pair.name) - 1] = '\0';

    stored_pair = aidb_list_front(&pair_list);
    AIDB_TEST_ASSERT_TRUE(stored_pair != NULL);
    AIDB_TEST_ASSERT_TRUE(stored_pair->id == 7);
    AIDB_TEST_ASSERT_TRUE(strcmp(stored_pair->name, "seven") == 0);

    aidb_list_deinit(&pair_list);
    return 0;
}

static int test_list_null_arguments(void)
{
    aidb_list list;
    int value = 42;
    int out = 0;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);

    AIDB_TEST_ASSERT_TRUE(aidb_list_size(NULL) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_list_is_empty(NULL));
    AIDB_TEST_ASSERT_TRUE(aidb_list_front(NULL) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_list_back(NULL) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_list_front_const(NULL) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_list_back_const(NULL) == NULL);

    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(NULL, &value) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_front(NULL, &value) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, NULL) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_front(&list, NULL) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(NULL, &out) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(NULL, &out) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_front(&list, NULL) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_list_pop_back(&list, NULL) == AIDB_ERROR_INVALID_ARGUMENT);

    aidb_list_clear(NULL);
    aidb_list_deinit(NULL);

    aidb_list_deinit(&list);
    return 0;
}

static int test_list_deinit(void)
{
    aidb_list list;
    int value = 42;

    AIDB_TEST_ASSERT_TRUE(aidb_list_init(&list, sizeof(int)) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(aidb_list_push_back(&list, &value) == AIDB_OK);

    aidb_list_deinit(&list);
    AIDB_TEST_ASSERT_TRUE(list.size == 0);
    AIDB_TEST_ASSERT_TRUE(list.head == NULL);
    AIDB_TEST_ASSERT_TRUE(list.tail == NULL);
    AIDB_TEST_ASSERT_TRUE(list.element_size == 0);

    return 0;
}

int main(void)
{
    AIDB_TEST_ASSERT_TRUE(test_list_init() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_push_back_order() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_push_front_order() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_pop_back_order() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_empty_pop() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_front_back_const() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_clear() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_struct_value_copy() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_null_arguments() == 0);
    AIDB_TEST_ASSERT_TRUE(test_list_deinit() == 0);

    return 0;
}
