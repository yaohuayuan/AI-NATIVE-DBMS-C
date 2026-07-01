#include "aidb/string_utils.h"

#include "aidb/memory.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

char *aidb_string_duplicate(const char *text) {
    if (text == NULL) {
        return NULL;
    }
    size_t len = strlen(text);
    if (len == SIZE_MAX) {
        return NULL;
    }
    char *result = aidb_malloc(len + 1);
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, text, len + 1);
    return result;
}

char *aidb_string_duplicate_n(const char *text, size_t length) {
    if (text == NULL) {
        return NULL;
    }
    size_t index = 0;
    while (index < length && text[index] != '\0') {
        index++;
    }
    if (index == SIZE_MAX) {
        return NULL;
    }
    char *result = aidb_malloc(index + 1);
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, text, index);
    result[index] = '\0';
    return result;
}

int aidb_string_equal(const char *left, const char *right) {
    if (left == NULL && right == NULL) {
        return 1;
    }
    if (left == NULL) {
        return 0;
    }
    if (right == NULL) {
        return 0;
    }
    return strcmp(left, right) == 0;
}

int aidb_string_starts_with(const char *text, const char *prefix) {
    if (text == NULL || prefix == NULL) {
        return 0;
    }
    size_t prefix_len = strlen(prefix);
    return strncmp(text, prefix, prefix_len) == 0;
}

int aidb_string_ends_with(const char *text, const char *suffix) {
    if (text == NULL || suffix == NULL) {
        return 0;
    }

    size_t text_len = strlen(text), suffix_len = strlen(suffix);
    if (text_len < suffix_len) {
        return 0;
    }
    return memcmp(text + text_len - suffix_len, suffix, suffix_len) == 0;
}

void aidb_string_to_lower_ascii(char *text) {
    if (text == NULL) {
        return;
    }
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = text[i] + ('a' - 'A');
        }
    }
}

void aidb_string_to_upper_ascii(char *text) {
    if (text == NULL) {
        return;
    }
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = text[i] + ('A' - 'a');
        }
    }
}

enum aidb_status aidb_string_concat(const char *left, const char *right, char **out_text) {
    if (out_text == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    if (left == NULL || right == NULL) {
        return AIDB_ERROR_INVALID_ARGUMENT;
    }

    size_t left_len = strlen(left), right_len = strlen(right);
    if (right_len > SIZE_MAX - left_len) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }
    size_t total_len = left_len + right_len;
    if (total_len == SIZE_MAX) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }
    size_t out_text_len = total_len + 1;
    char *result = aidb_malloc(out_text_len);
    if (result == NULL) {
        return AIDB_ERROR_OUT_OF_MEMORY;
    }
    memcpy(result, left, left_len);
    memcpy(result + left_len, right, right_len);
    result[left_len + right_len] = '\0';
    *out_text = result;
    return AIDB_OK;
}
