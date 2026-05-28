#include "aidb/aidb.h"

void aidb_init(AidbContext *ctx) {
    if (ctx == 0) {
        return;
    }

    ctx->project_name = "AI-NATIVE-DBMS-C";
    ctx->initialized = 1;
}

void aidb_shutdown(AidbContext *ctx) {
    if (ctx == 0) {
        return;
    }

    ctx->initialized = 0;
}

const char *aidb_version(void) {
    return "0.1.0";
}
