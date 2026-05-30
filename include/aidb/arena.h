#ifndef AIDB_ARENA_H
#define AIDB_ARENA_H

#include <stddef.h>

#include "aidb/error.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aidb_arena_block;

struct aidb_arena {
    struct aidb_arena_block *first_block;
    struct aidb_arena_block *current_block;
    size_t default_block_size;
};

enum aidb_status aidb_arena_init(struct aidb_arena *arena,size_t default_block_size);

void aidb_arena_deinit(struct aidb_arena *arena);

void aidb_arena_reset(struct aidb_arena *arena);

void *aidb_arena_alloc(struct aidb_arena *arena,size_t size);

#ifdef __cplusplus
}
#endif

#endif