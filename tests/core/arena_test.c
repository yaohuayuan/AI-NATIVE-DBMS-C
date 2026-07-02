#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "aidb/arena.h"
#include "aidb/error.h"
#include "platform/align.h"
#include "support/test_assert.h"

int main(void)
{
    struct aidb_arena arena;

    AIDB_TEST_ASSERT_TRUE(aidb_arena_init(NULL, 64) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(aidb_arena_init(&arena, 0) == AIDB_ERROR_INVALID_ARGUMENT);

    AIDB_TEST_ASSERT_TRUE(aidb_arena_init(&arena, 64) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(arena.first_block == NULL);
    AIDB_TEST_ASSERT_TRUE(arena.current_block == NULL);
    AIDB_TEST_ASSERT_TRUE(arena.default_block_size == 64);

    AIDB_TEST_ASSERT_TRUE(aidb_arena_alloc(NULL, 16) == NULL);
    AIDB_TEST_ASSERT_TRUE(aidb_arena_alloc(&arena, 0) == NULL);

    unsigned char *p1 = aidb_arena_alloc(&arena, 16);
    AIDB_TEST_ASSERT_TRUE(p1 != NULL);
    memset(p1, 0x11, 16);
    for (int i = 0; i < 16; ++i) {
        AIDB_TEST_ASSERT_TRUE(p1[i] == 0x11);
    }
    AIDB_TEST_ASSERT_TRUE(arena.first_block != NULL);
    AIDB_TEST_ASSERT_TRUE(arena.current_block != NULL);

    unsigned char *p2 = aidb_arena_alloc(&arena, 16);
    AIDB_TEST_ASSERT_TRUE(p2 != NULL);
    AIDB_TEST_ASSERT_TRUE(p2 != p1);
    memset(p2, 0x22, 16);
    for (int i = 0; i < 16; ++i) {
        AIDB_TEST_ASSERT_TRUE(p2[i] == 0x22);
    }

    unsigned char *big = aidb_arena_alloc(&arena, 128);
    AIDB_TEST_ASSERT_TRUE(big != NULL);
    memset(big, 0x33, 128);
    for (int i = 0; i < 128; ++i) {
        AIDB_TEST_ASSERT_TRUE(big[i] == 0x33);
    }

    aidb_arena_reset(&arena);
    unsigned char *after_reset = aidb_arena_alloc(&arena, 8);
    AIDB_TEST_ASSERT_TRUE(after_reset != NULL);
    memset(after_reset, 0x44, 8);
    for (int i = 0; i < 8; ++i) {
        AIDB_TEST_ASSERT_TRUE(after_reset[i] == 0x44);
    }

    aidb_arena_reset(NULL);
    aidb_arena_deinit(NULL);
    aidb_arena_deinit(&arena);
    AIDB_TEST_ASSERT_TRUE(arena.first_block == NULL);
    AIDB_TEST_ASSERT_TRUE(arena.current_block == NULL);
    AIDB_TEST_ASSERT_TRUE(arena.default_block_size == 0);

    struct aidb_arena align_arena;
    AIDB_TEST_ASSERT_TRUE(aidb_arena_init(&align_arena, 64) == AIDB_OK);

    void *align_p1 = aidb_arena_alloc(&align_arena, 1);
    AIDB_TEST_ASSERT_TRUE(align_p1 != NULL);

    void *align_p2 = aidb_arena_alloc(&align_arena, sizeof(union aidb_platform_max_align));
    AIDB_TEST_ASSERT_TRUE(align_p2 != NULL);
    AIDB_TEST_ASSERT_TRUE(((uintptr_t)align_p2 % AIDB_PLATFORM_MAX_ALIGN) == 0);

    union aidb_platform_max_align *aligned_value = (union aidb_platform_max_align *)align_p2;
    (void)aligned_value;

    unsigned char *aligned_bytes = align_p2;
    memset(aligned_bytes, 0x55, sizeof(union aidb_platform_max_align));
    for (size_t i = 0; i < sizeof(union aidb_platform_max_align); ++i) {
        AIDB_TEST_ASSERT_TRUE(aligned_bytes[i] == 0x55);
    }

    aidb_arena_deinit(&align_arena);

    printf("arena_test passed.\n");
    return 0;
}
