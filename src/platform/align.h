#ifndef AIDB_PLATFORM_ALIGN_H
#define AIDB_PLATFORM_ALIGN_H

/*
 * Internal alignment baseline for arena allocations.
 *
 * This is not a public API. It covers the ordinary scalar and pointer
 * alignment requirements needed by the current project.
 */
union aidb_platform_max_align {
    void *pointer_value;
    long long long_long_value;
    double double_value;
    long double long_double_value;
};

#if defined(_MSC_VER)
#define AIDB_PLATFORM_ALIGNOF(type) __alignof(type)
#else
#define AIDB_PLATFORM_ALIGNOF(type) _Alignof(type)
#endif

#define AIDB_PLATFORM_MAX_ALIGN AIDB_PLATFORM_ALIGNOF(union aidb_platform_max_align)
#endif