#include "aidb/arena.h"
#include "aidb/memory.h"
static size_t aidb_size_max(size_t left, size_t right)
{
    return left > right ? left : right;
}
struct aidb_arena_block{
    struct aidb_arena_block *next;
    size_t capacity;
    size_t used;
    unsigned char data[];
};
enum aidb_status aidb_arena_init(struct aidb_arena *arena,size_t default_block_size){
    if(arena==NULL || default_block_size <= 0){
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    arena->current_block = NULL;
    arena->default_block_size = default_block_size;
    arena->first_block = NULL;
    return AIDB_OK;
}

void aidb_arena_deinit(struct aidb_arena *arena){
    if(arena == NULL){
        return;
    }
    struct aidb_arena_block *head_block =  arena->first_block,*p;
    while(head_block){
        p = head_block->next;
        aidb_free(head_block);
        head_block = p;
    }
    arena->current_block = NULL;
    arena->first_block = NULL;
    arena->default_block_size = 0;
}

void aidb_arena_reset(struct aidb_arena *arena){
    if(arena == NULL){
        return ;
    }
    struct aidb_arena_block *head_block =  arena->first_block;
    while(head_block){
        head_block->used = 0;
        head_block = head_block->next;
    }
    arena->current_block = arena->first_block;
}
void *aidb_arena_alloc(struct aidb_arena *arena, size_t size)
{
    size_t capacity;
    struct aidb_arena_block *block;
    void *ptr;

    if (arena == NULL || size == 0) {
        return NULL;
    }

    if (arena->current_block == NULL) {
        capacity = aidb_size_max(size, arena->default_block_size);

        block = aidb_malloc(sizeof(struct aidb_arena_block) + capacity);
        if (block == NULL) {
            return NULL;
        }

        block->capacity = capacity;
        block->used = 0;
        block->next = NULL;

        arena->first_block = block;
        arena->current_block = block;
    }

    if (arena->current_block->capacity - arena->current_block->used < size) {
        capacity = aidb_size_max(size, arena->default_block_size);

        block = aidb_malloc(sizeof(struct aidb_arena_block) + capacity);
        if (block == NULL) {
            return NULL;
        }

        block->capacity = capacity;
        block->used = 0;
        block->next = NULL;

        arena->current_block->next = block;
        arena->current_block = block;
    }

    ptr = arena->current_block->data + arena->current_block->used;
    arena->current_block->used += size;

    return ptr;
}