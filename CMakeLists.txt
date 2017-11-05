cmake_minimum_required(VERSION 2.6)
project(Lisp)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options()
SET(CMAKE_CXX_FLAGS  "-g -O0 -Wall -Wextra -pedantic")

include_directories(include src)

set(SOURCES
        include/primitives.h        src/primitives.c
        include/repl.h              src/repl.c
        include/environment.h       src/environment.c
        include/parser.h            src/parser.c
        include/run_lisp.h          src/run_lisp.c
        include/list.h              src/evaluator.c
        include/evaluator.h         src/list.c
        src/main.c)

add_executable(lisp ${SOURCES})

include_directories(test)
set(TEST_SOURCES
        include/primitives.h        src/primitives.c
        include/repl.h              src/repl.c
        include/environment.h       src/environment.c
        include/parser.h            src/parser.c
        include/run_lisp.h          src/run_lisp.c
        include/list.h              src/evaluator.c
        include/evaluator.h         src/list.c
        test/test.c)

add_executable(test-lisp ${TEST_SOURCES})