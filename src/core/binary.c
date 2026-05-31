#include "aidb/binary.h"


uint16_t aidb_read_u16_le(const unsigned char *data){
    uint16_t value = ((uint16_t)data[0] | (uint16_t)data[1] << 8 );
    return value;
}
uint32_t aidb_read_u32_le(const unsigned char *data){
    uint32_t value = ((uint32_t)data[0] |  (uint32_t)data[1] << 8 |  (uint32_t)data[2] << 16 | (uint32_t)data[3] << 24);
    return value;
}
uint64_t aidb_read_u64_le(const unsigned char *data){
    uint64_t value = ((uint64_t)data[0]) | ((uint64_t)data[1] << 8) | ((uint64_t)data[2] << 16) | ((uint64_t)data[3] << 24) |   ((uint64_t)data[4] << 32) | ((uint64_t)data[5] << 40) | ((uint64_t)data[6] << 48) | ((uint64_t)data[7] << 56);
    return value;
}

void aidb_write_u16_le(unsigned char *data, uint16_t value){
    data[0] = (unsigned char)(value & 0xff);
    data[1] = (unsigned char)((value >> 8) & 0xff);
}
void aidb_write_u32_le(unsigned char *data, uint32_t value){
    data[0] = (unsigned char)(value & 0xff);
    data[1] = (unsigned char)((value >> 8) & 0xff);
    data[2] = (unsigned char)((value >> 16) & 0xff);
    data[3] = (unsigned char)((value >> 24) & 0xff);
}
void aidb_write_u64_le(unsigned char *data, uint64_t value){
    data[0] = (unsigned char)(value & 0xff);
    data[1] = (unsigned char)((value >> 8) & 0xff);
    data[2] = (unsigned char)((value >> 16) & 0xff);
    data[3] = (unsigned char)((value >> 24) & 0xff);
    data[4] = (unsigned char)((value >> 32) & 0xff);
    data[5] = (unsigned char)((value >> 40) & 0xff);
    data[6] = (unsigned char)((value >> 48) & 0xff);
    data[7] = (unsigned char)((value >> 56) & 0xff);
}

int16_t aidb_read_i16_le(const unsigned char *data){
    return (int16_t)aidb_read_u16_le(data);
}
int32_t aidb_read_i32_le(const unsigned char *data){
    return (int32_t)aidb_read_u32_le(data);
}
int64_t aidb_read_i64_le(const unsigned char *data){
    return (int64_t)aidb_read_u64_le(data);
}


void aidb_write_i16_le(unsigned char *data, int16_t value){
    aidb_write_u16_le(data,(uint16_t)value);
}
void aidb_write_i32_le(unsigned char *data, int32_t value){
    aidb_write_u32_le(data, (uint32_t)value);
}
void aidb_write_i64_le(unsigned char *data, int64_t value){
    aidb_write_u64_le(data,(uint64_t)value);
}