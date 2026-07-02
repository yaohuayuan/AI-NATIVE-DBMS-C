cmake_minimum_required(VERSION 3.25)

project(AI_NATIVE_DBMS_C
    VERSION 0.1.0
    DESCRIPTION "AI-NATIVE-DBMS-C: a C-based experimental DBMS prototype"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ------------------------------------------------------------
# Build type
# ------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" AIDB_BUILD_TYPE_LOWER)

# ------------------------------------------------------------
# Output directories
# ------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${AIDB_BUILD_TYPE_LOWER}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${AIDB_BUILD_TYPE_LOWER}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${AIDB_BUILD_TYPE_LOWER}/lib")

# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------
if(MINGW OR CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
    )
endif()

# ------------------------------------------------------------
# Library target
# ------------------------------------------------------------
add_library(aidb_core STATIC
    src/core/context.c
    src/core/error.c
    src/core/memory.c
    src/core/arena.c
    src/core/binary.c
    src/core/vector.c
    src/core/list.c
    src/core/string_utils.c
)

target_include_directories(aidb_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(aidb_core
    PRIVATE
        AIDB_BUILD
)

# ------------------------------------------------------------
# Executable target
# ------------------------------------------------------------
add_executable(aidb
    src/cli/main.c
)

target_link_libraries(aidb
    PRIVATE
        aidb_core
)

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
option(AIDB_BUILD_TESTS "Build AI-NATIVE-DBMS-C tests" ON)

if(AIDB_BUILD_TESTS)
    enable_testing()

    add_executable(basic_core_test
        tests/core/basic_core_test.c
    )

    target_link_libraries(basic_core_test
        PRIVATE
            aidb_core
    )

    target_include_directories(basic_core_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    add_test(
        NAME basic_core_test
        COMMAND basic_core_test
    )

    add_executable(error_test
        tests/core/error_test.c
    )

    target_link_libraries(error_test
        PRIVATE
            aidb_core
    )

    target_include_directories(error_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    add_test(
        NAME error_test
        COMMAND error_test
    )

    add_executable(context_test
        tests/core/context_test.c
    )

    target_link_libraries(context_test
        PRIVATE
            aidb_core
    )

    target_include_directories(context_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    add_test(
        NAME context_test
        COMMAND context_test
    )

    add_executable(memory_test
        tests/core/memory_test.c
    )

    target_include_directories(memory_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(memory_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME memory_test
        COMMAND memory_test
    )

    add_executable(arena_test
        tests/core/arena_test.c
    )

    target_include_directories(arena_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(arena_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME arena_test
        COMMAND arena_test
    )

    add_executable(binary_test
        tests/core/binary_test.c
    )

    target_include_directories(binary_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(binary_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME binary_test
        COMMAND binary_test
    )

    add_executable(vector_test
        tests/core/vector_test.c
    )

    target_include_directories(vector_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(vector_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME vector_test
        COMMAND vector_test
    )

    add_executable(list_test
        tests/core/list_test.c
    )

    target_include_directories(list_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(list_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME list_test
        COMMAND list_test
    )

    add_executable(string_utils_test
        tests/core/string_utils_test.c
    )

    target_include_directories(string_utils_test
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests
    )

    target_link_libraries(string_utils_test
        PRIVATE
            aidb_core
    )

    add_test(
        NAME string_utils_test
        COMMAND string_utils_test
    )
endif()
