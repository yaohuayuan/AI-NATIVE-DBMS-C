#ifndef AIDB_AIDB_H
#define AIDB_AIDB_H

#ifdef __cplusplus
extern "C" {
#endif

#define AIDB_VERSION_MAJOR 0
#define AIDB_VERSION_MINOR 1
#define AIDB_VERSION_PATCH 0

typedef struct AidbContext {
    const char *project_name;
    int initialized;
} AidbContext;

void aidb_init(AidbContext *ctx);
void aidb_shutdown(AidbContext *ctx);
const char *aidb_version(void);

#ifdef __cplusplus
}
#endif

#endif