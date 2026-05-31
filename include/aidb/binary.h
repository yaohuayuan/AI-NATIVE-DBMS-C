#ifndef AIDB_BINARY_H
#define AIDB_BINARY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * AIDB binary encoding helpers.
 *
 * Rules:
 * - All functions use little-endian byte order.
 * - The data pointer must point to enough writable/readable bytes.
 * - This layer does not check NULL or bounds.
 * - ByteBuffer/Page layers are responsible for parameter and bounds checking.
 */

/* unsigned integer read */
uint16_t aidb_read_u16_le(const unsigned char *data);
uint32_t aidb_read_u32_le(const unsigned char *data);
uint64_t aidb_read_u64_le(const unsigned char *data);

/* unsigned integer write */
void aidb_write_u16_le(unsigned char *data, uint16_t value);
void aidb_write_u32_le(unsigned char *data, uint32_t value);
void aidb_write_u64_le(unsigned char *data, uint64_t value);

/* signed integer read */
int16_t aidb_read_i16_le(const unsigned char *data);
int32_t aidb_read_i32_le(const unsigned char *data);
int64_t aidb_read_i64_le(const unsigned char *data);

/* signed integer write */
void aidb_write_i16_le(unsigned char *data, int16_t value);
void aidb_write_i32_le(unsigned char *data, int32_t value);
void aidb_write_i64_le(unsigned char *data, int64_t value);

#ifdef __cplusplus
}
#endif

#endif