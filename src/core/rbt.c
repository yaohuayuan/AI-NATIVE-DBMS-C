#include "aidb/rbt.h"
#include "aidb/memory.h"

enum aidb_rbt_color { AIDB_RBT_RED, AIDB_RBT_BLACK };
struct aidb_rbt_node {
    aidb_rbt_node *parent;
    aidb_rbt_node *left;
    aidb_rbt_node *right;
    void *payload;
    enum aidb_rbt_color color;
};
static void aidb_rbt_delete_fixup(
    aidb_rbt *tree,
    aidb_rbt_node *node
);

static void aidb_rbt_insert_fixup(aidb_rbt *tree, aidb_rbt_node *node);

enum aidb_status aidb_rbt_init(aidb_rbt *tree,
                               int (*compare)(const void *left, const void *right)) {
    if (tree == NULL || compare == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    tree->compare = compare;
    tree->root = NULL;
    tree->size = 0;
    tree->nil = NULL;
    aidb_rbt_node *node = aidb_malloc(sizeof(*node));
    if (node == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }
    node->color = AIDB_RBT_BLACK;
    node->left = node;
    node->right = node;
    node->parent = node;
    node->payload = NULL;
    tree->nil = node;
    tree->root = node;
    return AIDB_OK;
}
static aidb_rbt_node *aidb_rbt_node_create(aidb_rbt *tree, void *payload) {
    aidb_rbt_node *node = aidb_malloc(sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->color = AIDB_RBT_RED;
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = tree->nil;
    node->payload = payload;

    return node;
}
void *aidb_rbt_find(const aidb_rbt *tree, const void *payload) {
    if (tree == NULL || payload == NULL || tree->nil == NULL || tree->compare == NULL) {
        return NULL;
    }
    aidb_rbt_node *node = tree->root;
    while (node != tree->nil) {
        int comparison = tree->compare(payload, node->payload);
        if (comparison == 0) {
            return node->payload;
        }
        if (comparison < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}
enum aidb_status aidb_rbt_insert(aidb_rbt *tree, void *payload, bool *out_inserted) {
    if (tree == NULL || payload == NULL || out_inserted == NULL || tree->nil == NULL ||
        tree->compare == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    *out_inserted = false;
    aidb_rbt_node *current = tree->root, *parent = tree->nil;
    while (current != tree->nil) {
        parent = current;
        int comparison = tree->compare(payload, current->payload);
        if (comparison == 0) {
            return AIDB_OK;
        }
        if (comparison < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    aidb_rbt_node *node = aidb_rbt_node_create(tree, payload);
    if (node == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }
    node->parent = parent;
    if (parent == tree->nil) {
        tree->root = node;
    } else if (tree->compare(payload, parent->payload) < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    aidb_rbt_insert_fixup(tree, node);

    tree->size++;
    *out_inserted = true;
    return AIDB_OK;
}
static void aidb_rbt_left_rotate(aidb_rbt *tree, aidb_rbt_node *node) {
    aidb_rbt_node *pivot = node->right;
    if (pivot == tree->nil) {
        return;
    }
    if (node->parent == tree->nil) {
        tree->root = pivot;
    } else if (node->parent->left == node) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }
    pivot->parent = node->parent;
    node->parent = pivot;
    node->right = pivot->left;
    if (pivot->left != tree->nil) {
        node->right->parent = node;
    }
    pivot->left = node;
}
static void aidb_rbt_right_rotate(aidb_rbt *tree, aidb_rbt_node *node) {
    aidb_rbt_node *pivot = node->left;
    if (pivot == tree->nil) {
        return;
    }
    if (node->parent == tree->nil) {
        tree->root = pivot;
    } else if (node->parent->left == node) {
        node->parent->left = pivot;
    } else {
        node->parent->right = pivot;
    }
    pivot->parent = node->parent;
    node->parent = pivot;
    node->left = pivot->right;
    if (pivot->right != tree->nil) {
        node->left->parent = node;
    }
    pivot->right = node;
}
static void aidb_rbt_insert_fixup(aidb_rbt *tree, aidb_rbt_node *node) {

    while (node->parent->color == AIDB_RBT_RED) {
        if (node->parent == node->parent->parent->left) {
            aidb_rbt_node *uncle = node->parent->parent->right;
            aidb_rbt_node *grandparent = node->parent->parent;
            aidb_rbt_node *parent = node->parent;
            if (uncle->color == AIDB_RBT_RED) {

                grandparent->color = AIDB_RBT_RED;
                parent->color = AIDB_RBT_BLACK;
                uncle->color = AIDB_RBT_BLACK;

                node = grandparent;
            } else {
                if (node == parent->right) {
                    node = node->parent;
                    aidb_rbt_left_rotate(tree, node);
                }
                node->parent->color = AIDB_RBT_BLACK;
                node->parent->parent->color = AIDB_RBT_RED;
                aidb_rbt_right_rotate(tree, node->parent->parent);
            }
        } else {
            aidb_rbt_node *uncle = node->parent->parent->left;
            aidb_rbt_node *grandparent = node->parent->parent;
            aidb_rbt_node *parent = node->parent;
            if (uncle->color == AIDB_RBT_RED) {

                grandparent->color = AIDB_RBT_RED;
                parent->color = AIDB_RBT_BLACK;
                uncle->color = AIDB_RBT_BLACK;

                node = grandparent;
            } else {
                if (node == parent->left) {
                    node = node->parent;
                    aidb_rbt_right_rotate(tree, node);
                }
                node->parent->color = AIDB_RBT_BLACK;
                node->parent->parent->color = AIDB_RBT_RED;
                aidb_rbt_left_rotate(tree, node->parent->parent);
            }
        }
    }
    tree->root->color = AIDB_RBT_BLACK;
}
static void aidb_rbt_destroy_subtree(aidb_rbt *tree, aidb_rbt_node *node) {
    if (node == tree->nil) {
        return;
    }
    aidb_rbt_destroy_subtree(tree, node->left);
    aidb_rbt_destroy_subtree(tree, node->right);
    aidb_free(node);
}
void aidb_rbt_deinit(aidb_rbt *tree) {
    if (tree == NULL) {
        return;
    }
    if (tree->nil != NULL) {
        if (tree->root != NULL) {
            aidb_rbt_destroy_subtree(tree, tree->root);
        }
        aidb_free(tree->nil);
    }
    tree->compare = NULL;
    tree->nil = NULL;
    tree->root = NULL;
    tree->size = 0;
}
static size_t aidb_rbt_validate_black_height(const aidb_rbt *tree, const aidb_rbt_node *node) {
    if (node == tree->nil) {
        return 1;
    }

    if (node->left == NULL || node->right == NULL) {
        return 0;
    }

    if (node->color != AIDB_RBT_RED &&
        node->color != AIDB_RBT_BLACK) {
        return 0;
    }

    if (node->color == AIDB_RBT_RED &&
        (node->left->color == AIDB_RBT_RED ||
        node->right->color == AIDB_RBT_RED)) {
        return 0;
    }

    size_t left_height = aidb_rbt_validate_black_height(tree, node->left);
    size_t right_height = aidb_rbt_validate_black_height(tree, node->right);
    if (left_height == 0 || right_height == 0 || left_height != right_height) {
        return 0;
    }
    return left_height + (node->color == AIDB_RBT_BLACK);
}
static bool aidb_rbt_validate_structure(
    const aidb_rbt *tree,
    const aidb_rbt_node *node,
    const void *minimum,
    const void *maximum,
    size_t *count
) {
    if (node == tree->nil) {
        return true;
    }
    if (node == NULL || tree->compare == NULL || tree->root == NULL || node->left == NULL || node->right == NULL || node->parent == NULL || node->payload == NULL ) {
        return false;
    }
    if(minimum != NULL && tree->compare(node->payload,minimum)<=0){
        return false;
    }
    if (maximum != NULL &&
        tree->compare(node->payload, maximum) >= 0) {
        return false;
    }
    if (node->left != tree->nil &&
        node->left->parent != node) {
        return false;
    }
    if (node->right != tree->nil &&
        node->right->parent != node) {
        return false;
    }
    (*count)++;
    bool left_valid = aidb_rbt_validate_structure(
        tree,
        node->left,
        minimum,
        node->payload,
        count
    );

    bool right_valid = aidb_rbt_validate_structure(
        tree,
        node->right,
        node->payload,
        maximum,
        count
    );

    return left_valid && right_valid;
}
bool aidb_rbt_validate(const aidb_rbt *tree){
    if (tree == NULL ||
        tree->root == NULL ||
        tree->nil == NULL ||
        tree->compare == NULL) {
        return false;
    }
    if (tree->root->parent != tree->nil ||
        tree->root->color != AIDB_RBT_BLACK) {
        return false;
    }
    if(tree->nil->color != AIDB_RBT_BLACK ||
        tree->nil->left != tree->nil ||
        tree->nil->right != tree->nil ||
        tree->nil->parent != tree->nil ||
        tree->nil->payload != NULL){
            return false;
    }
    if (tree->root == tree->nil) {
        return tree->size == 0;
    }
    if (aidb_rbt_validate_black_height(tree, tree->root) == 0) {
        return false;
    }
    size_t count = 0;
    if (!aidb_rbt_validate_structure(
            tree,
            tree->root,
            NULL,
            NULL,
            &count)) {
        return false;
    }

    return count == tree->size;
}
static aidb_rbt_node *aidb_rbt_find_node(
    aidb_rbt *tree,
    const void *payload
) {
    aidb_rbt_node *node = tree->root;
    while (node != tree->nil) {
        int comparison = tree->compare(payload, node->payload);
        if (comparison == 0) {
            return node;
        }
        if (comparison < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return tree->nil;
}
static aidb_rbt_node *aidb_rbt_minimum(
    const aidb_rbt *tree,
    aidb_rbt_node *node
) {
    if (node == tree->nil) {
        return tree->nil;
    }
    while (node->left != tree->nil) {
        node = node->left;
    }
    return node;
}
static void aidb_rbt_transplant(
    aidb_rbt *tree,
    aidb_rbt_node *target,
    aidb_rbt_node *replacement
) {
    if (tree->root == target){
        tree->root = replacement;
    } else if (target == target->parent->left) {
        target->parent->left = replacement;
    } else {
        target->parent->right = replacement;
    }
    replacement->parent = target->parent;
}
enum aidb_status aidb_rbt_remove(
    aidb_rbt *tree,
    const void *payload,
    void **out_payload,
    bool *out_removed
) {
    if (tree == NULL || payload == NULL || out_payload == NULL || out_removed == NULL || tree->root == NULL || tree->nil == NULL || tree->compare == NULL){
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    *out_payload = NULL;
    *out_removed = false;

    aidb_rbt_node *target = aidb_rbt_find_node(tree, payload);
    if (target == tree->nil) {
        return AIDB_OK;
    }

    aidb_rbt_node *moved = target;
    enum aidb_rbt_color removed_color = moved->color;
    aidb_rbt_node *fixup_node;

    if(target->left == tree->nil){
        fixup_node = target->right;
        aidb_rbt_transplant(tree, target, target->right);
    } else if (target->right == tree->nil) {
        fixup_node = target->left;
        aidb_rbt_transplant(tree, target, target->left);
    } else {
        moved = aidb_rbt_minimum(tree, target->right);
        removed_color = moved->color;
        fixup_node = moved->right;
        if (moved->parent == target) {
            fixup_node->parent = moved;
        } else {
            aidb_rbt_transplant(tree, moved, moved->right);
            moved->right = target->right;
            moved->right->parent = moved;
        }
        aidb_rbt_transplant(tree, target, moved);
        moved->left = target->left;
        moved->left->parent = moved;
        moved->color = target->color;
    }

    if (removed_color == AIDB_RBT_BLACK) {
        aidb_rbt_delete_fixup(tree, fixup_node);
    }

    *out_payload = target->payload;
    aidb_free(target);
    tree->size--;
    *out_removed = true;
    tree->nil->parent = tree->nil;

    return AIDB_OK;

}
static void aidb_rbt_delete_fixup(
    aidb_rbt *tree,
    aidb_rbt_node *node
) {
    while (node != tree->root && node->color == AIDB_RBT_BLACK) {
        if (node == node->parent->left) {
            aidb_rbt_node *sibling = node->parent->right;
            if (sibling->color == AIDB_RBT_RED) {
                sibling->color = AIDB_RBT_BLACK;
                node->parent->color = AIDB_RBT_RED;
                aidb_rbt_left_rotate(tree, node->parent);
                sibling = node->parent->right;
            }
            if (sibling->color == AIDB_RBT_BLACK) {
                if (sibling->left->color == AIDB_RBT_BLACK && sibling->right->color == AIDB_RBT_BLACK) {
                    sibling->color = AIDB_RBT_RED;
                    node = node->parent;
                } else {
                    if (sibling->right->color == AIDB_RBT_BLACK) {
                        sibling->left->color = AIDB_RBT_BLACK;
                        sibling->color = AIDB_RBT_RED;
                        aidb_rbt_right_rotate(tree, sibling);
                        sibling = node->parent->right;
                    }
                    sibling->color = sibling->parent->color;
                    sibling->parent->color = AIDB_RBT_BLACK;
                    sibling->right->color = AIDB_RBT_BLACK;
                    aidb_rbt_left_rotate(tree, sibling->parent);
                    node = tree->root;
                }
            }
        } else {
            aidb_rbt_node *sibling = node->parent->left;

            if (sibling->color == AIDB_RBT_RED) {
                sibling->color = AIDB_RBT_BLACK;
                node->parent->color = AIDB_RBT_RED;
                aidb_rbt_right_rotate(tree, node->parent);
                sibling = node->parent->left;
            }

            if (sibling->right->color == AIDB_RBT_BLACK &&
                sibling->left->color == AIDB_RBT_BLACK) {
                sibling->color = AIDB_RBT_RED;
                node = node->parent;
            } else {
                if (sibling->left->color == AIDB_RBT_BLACK) {
                    sibling->right->color = AIDB_RBT_BLACK;
                    sibling->color = AIDB_RBT_RED;
                    aidb_rbt_left_rotate(tree, sibling);
                    sibling = node->parent->left;
                }

                sibling->color = node->parent->color;
                node->parent->color = AIDB_RBT_BLACK;
                sibling->left->color = AIDB_RBT_BLACK;
                aidb_rbt_right_rotate(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = AIDB_RBT_BLACK;
}
