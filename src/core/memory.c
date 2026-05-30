#include "aidb/memory.h"
#include "stdlib.h"
void *aidb_malloc(size_t size){
    return malloc(size);
}

void *aidb_calloc(size_t count, size_t size){
    return calloc(count,size);
}

void *aidb_realloc(void *ptr, size_t size){
    return realloc(ptr,size);
}

void aidb_free(void *ptr){
    free(ptr);
}