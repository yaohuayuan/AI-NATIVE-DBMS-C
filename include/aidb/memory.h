#ifndef AIDB_MEMORY_H
#define AIDB_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *aidb_malloc(size_t size);

void *aidb_calloc(size_t count, size_t size);

void *aidb_realloc(void *ptr, size_t size);

void aidb_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
