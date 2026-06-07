#ifndef AIDB_VECTOR_H
#define AIDB_VECTOR_H

#include <stddef.h>

#include "aidb/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aidb_vector aidb_vector;

struct aidb_vector {
    void *data;
    size_t size;
    size_t capacity;
    size_t element_size;
};

enum aidb_status aidb_vector_init(aidb_vector *vector, size_t element_size);
void aidb_vector_deinit(aidb_vector *vector);

size_t aidb_vector_size(const aidb_vector *vector);
size_t aidb_vector_capacity(const aidb_vector *vector);

enum aidb_status aidb_vector_reserve(aidb_vector *vector, size_t capacity);
enum aidb_status aidb_vector_push_back(aidb_vector *vector, const void *element);

void *aidb_vector_at(aidb_vector *vector, size_t index);
const void *aidb_vector_at_const(const aidb_vector *vector, size_t index);

void aidb_vector_clear(aidb_vector *vector);

#ifdef __cplusplus
}
#endif

#endif