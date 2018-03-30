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

# cmake -DCMAKE_BUILD_TYPE=Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Mode: Release")
    set(CMAKE_C_FLAGS  "-Ofast")
else()
    message(STATUS "Mode: Debug")
    set(CMAKE_C_FLAGS  "-g -O0 -Wall -Wextra -pedantic")
endif()

include_directories(include src)
set(COMMON_SRC
        include/lisp-objects.h      src/lisp-objects.c
        include/evaluator.h         src/list.c
        include/primitives.h        src/primitives.c
        include/closure.h           src/closure.c
        include/interpreter.h       src/interpreter.c
        include/parser.h            src/parser.c
        include/list.h              src/evaluator.c
        include/garbage-collector.h src/garbage-collector.c
        include/environment.h       src/environment.c
        include/clist.h             src/clist.c
        include/math.h              src/math.c
        include/repl.h              src/repl.c
        include/stack-trace.h       src/stack-trace.c)

set(LISP_SRC ${COMMON_SRC} src/main.c)
add_executable(lisp ${LISP_SRC})
target_link_libraries(lisp readline)

include_directories(test)
set(TEST_SRC
        test/eval-test.h        test/eval-test.c
        test/parse-test.h       test/parse-test.c
        test/test.h             test/test.c
        test/main-test.c)

set(TEST_SRC ${COMMON_SRC} ${TEST_SRC})
add_executable(test-lisp ${TEST_SRC})
target_link_libraries(test-lisp readline)