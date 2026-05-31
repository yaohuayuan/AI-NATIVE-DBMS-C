#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "aidb/binary.h"
#include "support/test_assert.h"

int main(void)
{
    unsigned char data[8] = {0};

    aidb_write_u16_le(data, 0x1234u);
    AIDB_TEST_ASSERT_TRUE(data[0] == 0x34u);
    AIDB_TEST_ASSERT_TRUE(data[1] == 0x12u);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u16_le(data) == 0x1234u);

    aidb_write_u32_le(data, 0x12345678u);
    AIDB_TEST_ASSERT_TRUE(data[0] == 0x78u);
    AIDB_TEST_ASSERT_TRUE(data[1] == 0x56u);
    AIDB_TEST_ASSERT_TRUE(data[2] == 0x34u);
    AIDB_TEST_ASSERT_TRUE(data[3] == 0x12u);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u32_le(data) == 0x12345678u);

    aidb_write_u64_le(data, UINT64_C(0x1122334455667788));
    AIDB_TEST_ASSERT_TRUE(data[0] == 0x88u);
    AIDB_TEST_ASSERT_TRUE(data[1] == 0x77u);
    AIDB_TEST_ASSERT_TRUE(data[2] == 0x66u);
    AIDB_TEST_ASSERT_TRUE(data[3] == 0x55u);
    AIDB_TEST_ASSERT_TRUE(data[4] == 0x44u);
    AIDB_TEST_ASSERT_TRUE(data[5] == 0x33u);
    AIDB_TEST_ASSERT_TRUE(data[6] == 0x22u);
    AIDB_TEST_ASSERT_TRUE(data[7] == 0x11u);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u64_le(data) == UINT64_C(0x1122334455667788));

    aidb_write_u16_le(data, UINT16_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u16_le(data) == UINT16_MAX);

    aidb_write_u32_le(data, UINT32_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u32_le(data) == UINT32_MAX);

    aidb_write_u64_le(data, UINT64_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_u64_le(data) == UINT64_MAX);

    aidb_write_i16_le(data, -1);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i16_le(data) == -1);
    aidb_write_i16_le(data, INT16_MIN);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i16_le(data) == INT16_MIN);
    aidb_write_i16_le(data, INT16_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i16_le(data) == INT16_MAX);

    aidb_write_i32_le(data, -1);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i32_le(data) == -1);
    aidb_write_i32_le(data, INT32_MIN);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i32_le(data) == INT32_MIN);
    aidb_write_i32_le(data, INT32_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i32_le(data) == INT32_MAX);

    aidb_write_i64_le(data, -1);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i64_le(data) == -1);
    aidb_write_i64_le(data, INT64_MIN);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i64_le(data) == INT64_MIN);
    aidb_write_i64_le(data, INT64_MAX);
    AIDB_TEST_ASSERT_TRUE(aidb_read_i64_le(data) == INT64_MAX);

    printf("binary_test passed.\n");
    return 0;
}
