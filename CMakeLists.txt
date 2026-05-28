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
)

target_include_directories(aidb_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
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

    add_test(
        NAME basic_core_test
        COMMAND basic_core_test
    )
endif()
