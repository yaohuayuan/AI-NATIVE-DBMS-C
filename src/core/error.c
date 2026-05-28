#include "aidb/error.h"


const char *aidb_status_name(enum aidb_status status)
{
    switch (status) {
        case AIDB_OK:
            return "AIDB_OK";
        case AIDB_ERROR_INVALID_ARGUMENT:
            return "AIDB_ERROR_INVALID_ARGUMENT";
        case AIDB_ERROR_OUT_OF_MEMORY:
            return "AIDB_ERROR_OUT_OF_MEMORY";
        case AIDB_ERROR_IO:
            return "AIDB_ERROR_IO";
        case AIDB_ERROR_PARSE:
            return "AIDB_ERROR_PARSE";
        case AIDB_ERROR_NOT_FOUND:
            return "AIDB_ERROR_NOT_FOUND";
        case AIDB_ERROR_INTERNAL:
            return "AIDB_ERROR_INTERNAL";
        default:
            return "AIDB_ERROR_UNKNOWN";
    }
}

const char *aidb_status_message(enum aidb_status status)
{
    switch (status) {
        case AIDB_OK:
            return "ok";
        case AIDB_ERROR_INVALID_ARGUMENT:
            return "invalid argument";
        case AIDB_ERROR_OUT_OF_MEMORY:
            return "out of memory";
        case AIDB_ERROR_IO:
            return "io error";
        case AIDB_ERROR_PARSE:
            return "parse error";
        case AIDB_ERROR_NOT_FOUND:
            return "not found";
        case AIDB_ERROR_INTERNAL:
            return "internal error";
        default:
            return "unknown error";
    }
}

bool aidb_status_is_ok(enum aidb_status status){
    return status == AIDB_OK;
}
bool aidb_status_is_error(enum aidb_status status){
    return status != AIDB_OK;
}