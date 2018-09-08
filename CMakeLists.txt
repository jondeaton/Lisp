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

find_library(readline REQUIRED)

# Compiler flags for debugging, releasing, and performance benchmarking
set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic -DDEBUG")
set(RELEASE_FLAGS -Ofast -DNDEBUG)
set(BENCH_FLAGS -Ofast -fno-omit-frame-pointer -std=c++17 -DNDEBUG)
set(TEST_FLAGS -std=c++17 -g -O0 -Wall -Wextra -pedantic -DDEBUG)

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
        lib/murmur3.h               lib/murmur3.c
        lib/permutations.h          lib/permutations.c
        lib/cset.h                  lib/cset.c
        lib/ops.h                   lib/ops.c)

set(LISP_SRC
        include/interpreter.h       src/interpreter.c
        include/lisp-objects.h      src/lisp-objects.c
        include/evaluator.h         src/evaluator.c
        include/primitives.h        src/primitives.c
        include/closure.h           src/closure.c
        include/parser.h            src/parser.c
        include/list.h              src/list.c
        include/memory-manager.h    src/memory-manager.c
        include/environment.h       src/environment.c
        include/math-lib.h          src/math-lib.c
        include/repl.h              src/repl.c
        include/stack-trace.h       src/stack-trace.c)

add_library(clib STATIC ${CLIB_SRC})
add_executable(lisp ${LISP_SRC} src/main.c)
target_link_libraries(lisp readline clib)

include_directories(test)
set(TEST_SRC
        test/eval-test.h        test/eval-test.c
        test/parse-test.h       test/parse-test.c
        test/test.h             test/test.c
        test/main-test.c
        test/alphabet.h)

set(TEST_SRC ${LISP_SRC} ${TEST_SRC} test/permutation-test.hpp)
add_executable(test-lisp ${TEST_SRC})
target_link_libraries(test-lisp readline clib)

find_library(gtest gtest)
if (gtest)
    include_directories(test ${gtest_INCLUDE_DIR})

    set(CLIB_TEST_SRC
            test/clib-test.cpp
            test/permutation-test.hpp
            test/cmap-test.hpp test/cset-test.hpp)

    add_executable(clib-test ${CLIB_SRC} ${CLIB_TEST_SRC})
    target_link_libraries(clib-test clib gtest pthread)
    target_compile_options(clib-test PUBLIC ${TEST_FLAGS})
else()
    message("Google Test not found")
endif()


# Performance Benchmarking
# Make sure to install Google Benchmark
find_package(benchmark)
if(benchmark_FOUND)
    include_directories(bench lib ${benchmark_INCLUDE_DIR})

    set(CLIB_BENCH_SRC
            bench/clib-bench.cpp bench/cmap-bench.hpp)

    add_executable(cmap-bench ${CLIB_BENCH_SRC})
    target_link_libraries(cmap-bench benchmark clib pthread)
    target_compile_options(cmap-bench PUBLIC ${BENCH_FLAGS})
else()
    message(WARNING "Google Benchmark not found. Not building performance benchmarking.")
endif()
