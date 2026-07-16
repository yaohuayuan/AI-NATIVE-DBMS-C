#ifndef AIDB_RBT_H
#define AIDB_RBT_H

#include <stddef.h>
#include <stdbool.h>

#include "aidb/error.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct aidb_rbt_node aidb_rbt_node;
typedef struct aidb_rbt aidb_rbt;

struct aidb_rbt {
    aidb_rbt_node *root;
    aidb_rbt_node *nil;
    size_t size;
    int (*compare)(const void *left, const void *right);
};

/**
 * Initialize a caller-owned red-black tree.
 *
 * The tree owns its internal sentinel and nodes. Payloads are borrowed and
 * must remain valid while stored in the tree.
 *
 * @param tree Caller-owned tree to initialize.
 * @param compare Ordering callback returning a negative value, zero, or a
 *                positive value when left is less than, equal to, or greater
 *                than right.
 * @return AIDB_OK on success, AIDB_ERROR_INVALID_ARGUMENT for invalid
 *         arguments, or AIDB_ERROR_OUT_OF_MEMORY if allocation fails.
 */
enum aidb_status aidb_rbt_init(
    aidb_rbt *tree,
    int (*compare)(const void *left, const void *right)
);

/**
 * Find a payload that compares equal to the search payload.
 *
 * @param tree Initialized tree to search.
 * @param payload Borrowed search payload. It is not stored or released.
 * @return Borrowed stored payload if found, or NULL if not found or invalid.
 */
void *aidb_rbt_find(
    const aidb_rbt *tree,
    const void *payload
);

/**
 * Release all internal nodes and the sentinel owned by a tree.
 *
 * Stored payloads are borrowed and are not released.
 *
 * @param tree Tree to deinitialize.
 */
void aidb_rbt_deinit(aidb_rbt *tree);

/**
 * Insert a borrowed payload into a tree.
 *
 * A payload that compares equal to an existing payload is not inserted.
 * The tree does not copy or release payloads.
 *
 * @param tree Initialized tree receiving the payload.
 * @param payload Borrowed non-NULL payload to store.
 * @param out_inserted Set to true when inserted and false when an equal
 *                     payload exists or insertion fails.
 * @return AIDB_OK on insertion or duplicate detection,
 *         AIDB_ERROR_INVALID_ARGUMENT for invalid arguments, or
 *         AIDB_ERROR_OUT_OF_MEMORY if node allocation fails.
 */
enum aidb_status aidb_rbt_insert(
    aidb_rbt *tree,
    void *payload,
    bool *out_inserted
);

bool aidb_rbt_validate(const aidb_rbt *tree);

/**
 * Remove the payload that compares equal to a search payload.
 *
 * The tree releases its internal node but does not release the borrowed
 * payload. Removing a missing payload is a successful no-op.
 *
 * @param tree Initialized tree to modify.
 * @param payload Borrowed non-NULL search payload.
 * @param out_payload Receives the borrowed stored payload, or NULL when no
 *                    equal payload exists.
 * @param out_removed Set to true when a node is removed and false otherwise.
 * @return AIDB_OK on removal or when no equal payload exists, or
 *         AIDB_ERROR_INVALID_ARGUMENT for invalid arguments.
 */
enum aidb_status aidb_rbt_remove(
    aidb_rbt *tree,
    const void *payload,
    void **out_payload,
    bool *out_removed
);

#ifdef __cplusplus
}
#endif

#endif
