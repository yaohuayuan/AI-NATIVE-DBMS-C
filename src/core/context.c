#include "aidb/context.h"
#include <stdio.h>

enum aidb_status aidb_context_init(struct aidb_context *context)
{
    if (context == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    context->last_status = AIDB_OK;
    context->last_error_message[0] = '\0';

    return AIDB_OK;
}

void aidb_context_deinit(struct aidb_context *context)
{
    if (context == NULL) {
        return;
    }

    aidb_context_clear_error(context);
}

void aidb_context_clear_error(struct aidb_context *context)
{
    if (context == NULL) {
        return;
    }

    context->last_status = AIDB_OK;
    context->last_error_message[0] = '\0';
}

enum aidb_status aidb_context_set_error(
    struct aidb_context *context,
    enum aidb_status status,
    const char *message
)
{
    const char *actual_message = message;

    if (context == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    if (actual_message == NULL) {
        actual_message = aidb_status_message(status);
    }

    context->last_status = status;
    snprintf(
        context->last_error_message,
        AIDB_CONTEXT_ERROR_MESSAGE_CAPACITY,
        "%s",
        actual_message
    );

    return AIDB_OK;
}

enum aidb_status aidb_context_last_status(const struct aidb_context *context)
{
    if (context == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    return context->last_status;
}

const char *aidb_context_error_message(const struct aidb_context *context)
{
    if (context == NULL) {
        return "null context";
    }

    return context->last_error_message;
}