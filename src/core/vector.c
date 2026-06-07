#include "aidb/vector.h"
#include "aidb/memory.h"
#include <stdint.h>
#include <string.h>
#define AIDB_VECTOR_INITIAL_CAPACITY 4
enum aidb_status aidb_vector_init(aidb_vector *vector, size_t element_size){
    if(vector == NULL || element_size == 0){
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    vector->element_size = element_size;
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    return AIDB_OK;
}
void aidb_vector_deinit(aidb_vector *vector){
    if(vector == NULL){
        return ;
    }
    vector->element_size = 0;
    aidb_free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

size_t aidb_vector_size(const aidb_vector *vector){
    if(vector == NULL){
        return 0;
    }
    return vector->size;
}
size_t aidb_vector_capacity(const aidb_vector *vector){
    if(vector ==  NULL){
        return 0;
    }
    return vector->capacity;
}

enum aidb_status aidb_vector_reserve(aidb_vector *vector, size_t capacity)
{
    void *data;

    if (vector == NULL || vector->element_size == 0) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    if (capacity <= vector->capacity) {
        return AIDB_OK;
    }

    if (capacity > SIZE_MAX / vector->element_size) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    data = aidb_realloc(vector->data, capacity * vector->element_size);
    if (data == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }

    vector->data = data;
    vector->capacity = capacity;

    return AIDB_OK;
}
enum aidb_status aidb_vector_push_back(aidb_vector *vector, const void *element)
{
    size_t new_capacity;
    enum aidb_status status;
    void *dest;

    if (vector == NULL || element == NULL || vector->element_size == 0) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    if (vector->size == vector->capacity) {
        if (vector->capacity == 0) {
            new_capacity = AIDB_VECTOR_INITIAL_CAPACITY;
        } else {
            if (vector->capacity > SIZE_MAX / 2) {
                return AIDB_ERROR_OUT_OF_MEMORY;
            }

            new_capacity = vector->capacity * 2;
        }

        status = aidb_vector_reserve(vector, new_capacity);
        if (status != AIDB_OK) {
            return status;
        }
    }

    dest = (unsigned char *)vector->data + vector->size * vector->element_size;
    memcpy(dest, element, vector->element_size);

    vector->size++;
    return AIDB_OK;
}

void *aidb_vector_at(aidb_vector *vector, size_t index){
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    return (unsigned char *)vector->data + index * vector->element_size;
}
const void *aidb_vector_at_const(const aidb_vector *vector, size_t index){
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    return (const unsigned char *)vector->data + index * vector->element_size;
}

void aidb_vector_clear(aidb_vector *vector){
    if(vector == NULL){
        return;
    }
    vector->size = 0;
}