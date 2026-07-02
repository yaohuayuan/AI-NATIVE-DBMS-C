#include "aidb/arena.h"
#include "aidb/memory.h"
#include "platform/align.h"

#include <stddef.h>
#include <stdint.h>
static size_t aidb_align_up(size_t value, size_t alignment)
{
    size_t remainder = value % alignment;

    if (remainder == 0) {
        return value;
    }

    return value + (alignment - remainder);
}
static size_t aidb_size_max(size_t left, size_t right)
{
    return left > right ? left : right;
}
struct aidb_arena_block{
    struct aidb_arena_block *next;
    size_t capacity;
    size_t used;
    union aidb_platform_max_align alignment_padding;
    unsigned char data[];
};
enum aidb_status aidb_arena_init(struct aidb_arena *arena,size_t default_block_size){
    if(arena==NULL || default_block_size == 0){
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
    size_t alignment;
    size_t aligned_used;
    void *ptr;

    if (arena == NULL || size == 0) {
        return NULL;
    }
    alignment = AIDB_PLATFORM_MAX_ALIGN;
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

    
    aligned_used = aidb_align_up(arena->current_block->used, alignment);
    if ( aligned_used > arena->current_block->capacity ||
        arena->current_block->capacity - aligned_used < size) {
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
    aligned_used = aidb_align_up(arena->current_block->used, alignment);
    ptr = arena->current_block->data + aligned_used;
    arena->current_block->used = aligned_used + size;

    return ptr;
}