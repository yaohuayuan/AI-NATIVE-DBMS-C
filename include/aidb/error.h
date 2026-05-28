#ifndef AIDB_ERROR_H
#define AIDB_ERROR_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

enum aidb_status {
    AIDB_OK = 0,
    AIDB_ERROR_INVALID_ARGUMENT,
    AIDB_ERROR_OUT_OF_MEMORY,
    AIDB_ERROR_IO,
    AIDB_ERROR_PARSE,
    AIDB_ERROR_NOT_FOUND,
    AIDB_ERROR_INTERNAL
};

const char *aidb_status_name(enum aidb_status status);
const char *aidb_status_message(enum aidb_status status);

bool aidb_status_is_ok(enum aidb_status status);
bool aidb_status_is_error(enum aidb_status status);

#ifdef __cplusplus
}
#endif

#endif