#include <stdio.h>

#include "aidb/aidb.h"

int main(void) {
    AidbContext ctx;

    aidb_init(&ctx);

    printf("====================================\n");
    printf(" %s\n", ctx.project_name);
    printf(" Version: %s\n", aidb_version());
    printf(" Status : %s\n", ctx.initialized ? "initialized" : "not initialized");
    printf("====================================\n");

    aidb_shutdown(&ctx);

    return 0;
}
