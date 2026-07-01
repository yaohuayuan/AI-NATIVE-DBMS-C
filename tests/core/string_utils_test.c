#include <stddef.h>
#include <string.h>

#include "aidb/error.h"
#include "aidb/memory.h"
#include "aidb/string_utils.h"
#include "support/test_assert.h"

static int test_string_duplicate(void)
{
    const char source[] = "hello";
    char *copy;

    AIDB_TEST_ASSERT_TRUE(aidb_string_duplicate(NULL) == NULL);

    copy = aidb_string_duplicate(source);
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "hello") == 0);
    AIDB_TEST_ASSERT_TRUE(copy != source);
    aidb_free(copy);

    copy = aidb_string_duplicate("");
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "") == 0);
    aidb_free(copy);

    return 0;
}

static int test_string_duplicate_n(void)
{
    char *copy;
    const char raw[] = {'a', 'b', 'c', 'd', 'e'};
    const char embedded_null[] = {'a', 'b', '\0', 'c', 'd'};

    AIDB_TEST_ASSERT_TRUE(aidb_string_duplicate_n(NULL, 3) == NULL);

    copy = aidb_string_duplicate_n("abcdef", 3);
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "abc") == 0);
    aidb_free(copy);

    copy = aidb_string_duplicate_n("abc", 7);
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "abc") == 0);
    aidb_free(copy);

    copy = aidb_string_duplicate_n("abc", 0);
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "") == 0);
    aidb_free(copy);

    copy = aidb_string_duplicate_n(raw, 3);
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "abc") == 0);
    aidb_free(copy);

    copy = aidb_string_duplicate_n(embedded_null, sizeof(embedded_null));
    AIDB_TEST_ASSERT_TRUE(copy != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(copy, "ab") == 0);
    aidb_free(copy);

    return 0;
}

static int test_string_equal(void)
{
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal(NULL, NULL) == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal(NULL, "abc") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal("abc", NULL) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal("abc", "abc") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal("abc", "abcd") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_equal("abc", "abC") == 0);

    return 0;
}

static int test_string_starts_with(void)
{
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with(NULL, "a") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", NULL) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", "") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", "a") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", "abc") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", "abcd") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_starts_with("abc", "bc") == 0);

    return 0;
}

static int test_string_ends_with(void)
{
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with(NULL, "c") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", NULL) == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", "") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", "c") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", "abc") == 1);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", "abcd") == 0);
    AIDB_TEST_ASSERT_TRUE(aidb_string_ends_with("abc", "ab") == 0);

    return 0;
}

static int test_string_case_conversion(void)
{
    char lower_text[] = "AbC-123_Z!";
    char upper_text[] = "aBc-123_z!";

    aidb_string_to_lower_ascii(NULL);
    aidb_string_to_upper_ascii(NULL);

    aidb_string_to_lower_ascii(lower_text);
    AIDB_TEST_ASSERT_TRUE(strcmp(lower_text, "abc-123_z!") == 0);

    aidb_string_to_upper_ascii(upper_text);
    AIDB_TEST_ASSERT_TRUE(strcmp(upper_text, "ABC-123_Z!") == 0);

    return 0;
}

static int test_string_concat(void)
{
    char *out;

    out = (char *)1;
    AIDB_TEST_ASSERT_TRUE(aidb_string_concat(NULL, "right", &out) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(out == NULL);

    out = (char *)1;
    AIDB_TEST_ASSERT_TRUE(aidb_string_concat("left", NULL, &out) == AIDB_ERROR_INVALID_ARGUMENT);
    AIDB_TEST_ASSERT_TRUE(out == NULL);

    AIDB_TEST_ASSERT_TRUE(aidb_string_concat("left", "right", NULL) ==
                          AIDB_ERROR_INVALID_ARGUMENT);

    out = NULL;
    AIDB_TEST_ASSERT_TRUE(aidb_string_concat("left", "right", &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(out, "leftright") == 0);
    aidb_free(out);

    out = NULL;
    AIDB_TEST_ASSERT_TRUE(aidb_string_concat("", "right", &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(out, "right") == 0);
    aidb_free(out);

    out = NULL;
    AIDB_TEST_ASSERT_TRUE(aidb_string_concat("left", "", &out) == AIDB_OK);
    AIDB_TEST_ASSERT_TRUE(out != NULL);
    AIDB_TEST_ASSERT_TRUE(strcmp(out, "left") == 0);
    aidb_free(out);

    return 0;
}

int main(void)
{
    AIDB_TEST_ASSERT_TRUE(test_string_duplicate() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_duplicate_n() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_equal() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_starts_with() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_ends_with() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_case_conversion() == 0);
    AIDB_TEST_ASSERT_TRUE(test_string_concat() == 0);

    return 0;
}
