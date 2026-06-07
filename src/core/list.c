#include "aidb/list.h"
#include "aidb/memory.h"

#include <stdint.h>
#include <string.h>

struct aidb_list_node {
    aidb_list_node *prev;
    aidb_list_node *next;
    unsigned char data[];
};

static aidb_list_node *aidb_list_create_sentinel_node(void)
{
    aidb_list_node *node = aidb_malloc(sizeof(aidb_list_node));
    if (node == NULL) {
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    return node;
}

static aidb_list_node *aidb_list_create_value_node(
    const aidb_list *list,
    const void *element
)
{
    aidb_list_node *node;

    if (list == NULL || element == NULL || list->element_size == 0) {
        return NULL;
    }

    if (list->element_size > SIZE_MAX - sizeof(aidb_list_node)) {
        return NULL;
    }

    node = aidb_malloc(sizeof(aidb_list_node) + list->element_size);
    if (node == NULL) {
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    memcpy(node->data, element, list->element_size);

    return node;
}

static void aidb_list_insert_between(
    aidb_list_node *prev,
    aidb_list_node *next,
    aidb_list_node *node
)
{
    node->prev = prev;
    node->next = next;
    prev->next = node;
    next->prev = node;
}

static void aidb_list_unlink_node(aidb_list_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void aidb_list_destroy_value_node(aidb_list_node *node)
{
    if (node == NULL) {
        return;
    }

    aidb_list_unlink_node(node);
    aidb_free(node);
}

static bool aidb_list_is_ready(const aidb_list *list)
{
    return list != NULL &&
           list->head != NULL &&
           list->tail != NULL &&
           list->element_size != 0;
}

enum aidb_status aidb_list_init(aidb_list *list, size_t element_size)
{
    if (list == NULL || element_size == 0) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    list->head = aidb_list_create_sentinel_node();
    if (list->head == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    list->tail = aidb_list_create_sentinel_node();
    if (list->tail == NULL) {
        aidb_free(list->head);
        list->head = NULL;
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->size = 0;
    list->element_size = element_size;

    return AIDB_OK;
}

void aidb_list_clear(aidb_list *list)
{
    aidb_list_node *node;

    if (!aidb_list_is_ready(list)) {
        return;
    }

    node = list->head->next;
    while (node != list->tail) {
        aidb_list_node *next = node->next;
        aidb_free(node);
        node = next;
    }

    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->size = 0;
}

void aidb_list_deinit(aidb_list *list)
{
    if (list == NULL) {
        return;
    }

    if (list->head != NULL && list->tail != NULL) {
        aidb_list_clear(list);
    }

    aidb_free(list->head);
    aidb_free(list->tail);

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->element_size = 0;
}

size_t aidb_list_size(const aidb_list *list)
{
    if (list == NULL) {
        return 0;
    }

    return list->size;
}

bool aidb_list_is_empty(const aidb_list *list)
{
    return list == NULL || list->size == 0;
}

enum aidb_status aidb_list_push_front(aidb_list *list, const void *element)
{
    aidb_list_node *node;

    if (!aidb_list_is_ready(list) || element == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    node = aidb_list_create_value_node(list, element);
    if (node == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    aidb_list_insert_between(list->head, list->head->next, node);
    list->size++;

    return AIDB_OK;
}

enum aidb_status aidb_list_push_back(aidb_list *list, const void *element)
{
    aidb_list_node *node;

    if (!aidb_list_is_ready(list) || element == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    node = aidb_list_create_value_node(list, element);
    if (node == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    aidb_list_insert_between(list->tail->prev, list->tail, node);
    list->size++;

    return AIDB_OK;
}

enum aidb_status aidb_list_pop_front(aidb_list *list, void *out_element)
{
    aidb_list_node *node;

    if (!aidb_list_is_ready(list) || out_element == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    if (list->size == 0) {
        return AIDB_ERROR_NOT_FOUND;
    }

    node = list->head->next;
    memcpy(out_element, node->data, list->element_size);

    aidb_list_destroy_value_node(node);
    list->size--;

    return AIDB_OK;
}

enum aidb_status aidb_list_pop_back(aidb_list *list, void *out_element)
{
    aidb_list_node *node;

    if (!aidb_list_is_ready(list) || out_element == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    if (list->size == 0) {
        return AIDB_ERROR_NOT_FOUND;
    }

    node = list->tail->prev;
    memcpy(out_element, node->data, list->element_size);

    aidb_list_destroy_value_node(node);
    list->size--;

    return AIDB_OK;
}

void *aidb_list_front(aidb_list *list)
{
    if (!aidb_list_is_ready(list) || list->size == 0) {
        return NULL;
    }

    return list->head->next->data;
}

const void *aidb_list_front_const(const aidb_list *list)
{
    if (!aidb_list_is_ready(list) || list->size == 0) {
        return NULL;
    }

    return list->head->next->data;
}

void *aidb_list_back(aidb_list *list)
{
    if (!aidb_list_is_ready(list) || list->size == 0) {
        return NULL;
    }

    return list->tail->prev->data;
}

const void *aidb_list_back_const(const aidb_list *list)
{
    if (!aidb_list_is_ready(list) || list->size == 0) {
        return NULL;
    }

    return list->tail->prev->data;
}