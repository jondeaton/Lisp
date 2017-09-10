cmake_minimum_required(VERSION 2.6)
project(Lisp)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options()
SET(CMAKE_CXX_FLAGS  "-g -O0 -Wall -Wextra -pedantic")

include_directories(include src)

set(SOURCES
        include/lisp.h
        include/repl.h
        include/environment.h
        include/parser.h
        include/run_lisp.h
        include/list.h
        include/evaluator.h
        src/lisp.c
        src/repl.c
        src/environment.c
        src/parser.c
        src/run_lisp.c
        src/evaluator.c
        src/list.c
        src/main.c)

add_executable(lisp.o ${SOURCES})

include_directories(test)
set(TEST_SOURCES
        include/lisp.h
        include/repl.h
        include/environment.h
        include/evaluator.h
        include/parser.h
        include/run_lisp.h
        include/list.h
        src/lisp.c
        src/repl.c
        src/environment.c
        src/evaluator.c
        src/parser.c
        src/run_lisp.c
        src/list.c
        test/test.c)

add_executable(test.o ${TEST_SOURCES})