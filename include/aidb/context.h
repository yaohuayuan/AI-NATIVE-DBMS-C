#ifndef AIDB_CONTEXT_H
#define AIDB_CONTEXT_H

#include "aidb/error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AIDB_CONTEXT_ERROR_MESSAGE_CAPACITY 256

struct aidb_context {
    enum aidb_status last_status;
    char last_error_message[AIDB_CONTEXT_ERROR_MESSAGE_CAPACITY];
};

enum aidb_status aidb_context_init(struct aidb_context *context);
void aidb_context_deinit(struct aidb_context *context);

void aidb_context_clear_error(struct aidb_context *context);

enum aidb_status aidb_context_set_error(
    struct aidb_context *context,
    enum aidb_status status,
    const char *message
);

enum aidb_status aidb_context_last_status(const struct aidb_context *context);

const char *aidb_context_error_message(const struct aidb_context *context);

#ifdef __cplusplus
}
#endif

#endif