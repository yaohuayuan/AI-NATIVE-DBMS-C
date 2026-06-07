#ifndef AIDB_LIST_H
#define AIDB_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "aidb/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aidb_list_node aidb_list_node;
typedef struct aidb_list aidb_list;

struct aidb_list {
    aidb_list_node *head;
    aidb_list_node *tail;
    size_t size;
    size_t element_size;
};

enum aidb_status aidb_list_init(aidb_list *list, size_t element_size);
void aidb_list_deinit(aidb_list *list);

size_t aidb_list_size(const aidb_list *list);
bool aidb_list_is_empty(const aidb_list *list);

enum aidb_status aidb_list_push_front(aidb_list *list, const void *element);
enum aidb_status aidb_list_push_back(aidb_list *list, const void *element);

enum aidb_status aidb_list_pop_front(aidb_list *list, void *out_element);
enum aidb_status aidb_list_pop_back(aidb_list *list, void *out_element);

void *aidb_list_front(aidb_list *list);
const void *aidb_list_front_const(const aidb_list *list);

void *aidb_list_back(aidb_list *list);
const void *aidb_list_back_const(const aidb_list *list);

void aidb_list_clear(aidb_list *list);

#ifdef __cplusplus
}
#endif

#endif