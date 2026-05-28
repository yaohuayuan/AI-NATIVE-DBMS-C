#include <stdio.h>

#include "aidb/aidb.h"

int main(void)
{
    struct aidb_context context;

    if (aidb_context_init(&context) != AIDB_OK) {
        fprintf(stderr, "failed to initialize aidb context\n");
        return 1;
    }

    printf("====================================\n");
    printf(" AI-NATIVE-DBMS-C\n");
    printf(" Status : %s\n", aidb_status_message(aidb_context_last_status(&context)));
    printf("====================================\n");

    aidb_context_deinit(&context);

    return 0;
}