#ifndef AIDB_STRING_UTILS_H
#define AIDB_STRING_UTILS_H

#include <stddef.h>

#include "aidb/error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * String utility helpers.
 *
 * Rules:
 * - Functions returning char * return owned memory.
 * - Owned strings must be released by aidb_free.
 * - Functions in this module do not implement full Unicode handling.
 * - ASCII case conversion only handles English letters A-Z / a-z.
 */

/*
 * Duplicate a null-terminated string.
 *
 * Returns:
 * - owned copied string on success
 * - NULL if text is NULL or allocation fails
 */
char *aidb_string_duplicate(const char *text);

/*
 * Duplicate at most length bytes from text and append '\0'.
 *
 * The input text does not need to contain '\0' within length bytes.
 *
 * Returns:
 * - owned copied string on success
 * - NULL if text is NULL or allocation fails
 */
char *aidb_string_duplicate_n(const char *text, size_t length);

/*
 * Compare two strings for equality.
 *
 * Rules:
 * - returns 1 if both strings are non-NULL and equal
 * - returns 1 if both are NULL
 * - returns 0 otherwise
 */
int aidb_string_equal(const char *left, const char *right);

/*
 * Check whether text starts with prefix.
 *
 * Rules:
 * - returns 1 if prefix is empty
 * - returns 0 if text or prefix is NULL
 */
int aidb_string_starts_with(const char *text, const char *prefix);

/*
 * Check whether text ends with suffix.
 *
 * Rules:
 * - returns 1 if suffix is empty
 * - returns 0 if text or suffix is NULL
 */
int aidb_string_ends_with(const char *text, const char *suffix);

/*
 * Convert ASCII letters in text to lowercase in place.
 *
 * NULL input is ignored.
 */
void aidb_string_to_lower_ascii(char *text);

/*
 * Convert ASCII letters in text to uppercase in place.
 *
 * NULL input is ignored.
 */
void aidb_string_to_upper_ascii(char *text);

/*
 * Concatenate two null-terminated strings.
 *
 * out_text receives an owned string on success.
 * Caller must release it with aidb_free.
 *
 * Rules:
 * - left, right, and out_text must not be NULL
 * - *out_text is set to NULL on failure
 */
enum aidb_status aidb_string_concat(
    const char *left,
    const char *right,
    char **out_text
);

#ifdef __cplusplus
}
#endif

#endif