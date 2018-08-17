set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
cmake_minimum_required(VERSION 2.6)
project(Lisp)

macro(use_c99)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
        endif ()
    else ()
        set (CMAKE_C_STANDARD 99)
    endif ()
endmacro(use_c99)
use_c99()

# Compiler flags for debugging, releasing, and performance benchmarking
set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic -DDEBUG")
set(RELEASE_FLAGS "-Ofast")
set(BENCH_FLAGS "-O3 -fno-omit-frame-pointer")

# To configure for "release", then run cmake with:
# cmake -DCMAKE_BUILD_TYPE=Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Mode: Release")
    set(CMAKE_C_FLAGS  ${RELEASE_FLAGS})
else()
    message(STATUS "Mode: Debug")
    set(CMAKE_C_FLAGS ${DEBUG_FLAGS})
endif()

include_directories(include src lib)
set(CLIB_SRC
        lib/cvector.h               lib/cvector.c
        lib/clist.h                 lib/clist.c
        lib/cmap.h                  lib/cmap.c
        lib/hash.h                  lib/hash.c
        lib/murmur3.h               lib/murmur3.c)

set(COMMON_SRC
        include/lisp-objects.h      src/lisp-objects.c
        include/evaluator.h         src/evaluator.c
        include/primitives.h        src/primitives.c
        include/closure.h           src/closure.c
        include/interpreter.h       src/interpreter.c
        include/parser.h            src/parser.c
        include/list.h              src/list.c
        include/memory-manager.h    src/memory-manager.c
        include/environment.h       src/environment.c
        include/math.h              src/math.c
        include/repl.h              src/repl.c
        include/stack-trace.h       src/stack-trace.c)

set(LISP_SRC ${COMMON_SRC} src/main.c)

add_library(clib STATIC ${CLIB_SRC})
add_executable(lisp ${LISP_SRC})
target_link_libraries(lisp readline clib)

include_directories(test)
set(TEST_SRC
        test/eval-test.h        test/eval-test.c
        test/parse-test.h       test/parse-test.c
        test/test.h             test/test.c
        test/main-test.c)

set(TEST_SRC ${COMMON_SRC} ${TEST_SRC})
add_executable(test-lisp ${TEST_SRC})
target_link_libraries(test-lisp readline clib)

set(CLIB_TEST_SRC
        test/clib-test.c
        test/cmap-test.h        test/cmap-test.c
        test/permutations.h     test/permutations.c)

add_executable(clib-test ${CLIB_SRC} ${CLIB_TEST_SRC})
target_link_libraries(clib-test clib)

# Performance Benchmarking
find_package(GoogleBenchmark)
if(GoogleBenchmark_FOUND)
    include_directories(bench ${GoogleBenchmark_INCLUDE_DIR})
        set(CLIB_BENCH_SRC
                bench/map-bench.c)
        add_executable(cmap-bench ${CLIB_BENCH_SRC})
    target_link_libraries(cmap-bench ${GoogleBenchmark_INCLUDE_DIR})
    target_compile_options(cmap-bench ${BENCH_FLAGS})
else()
    message("Google Benchmark not found. Not building performance benchmarking.")
endif()
