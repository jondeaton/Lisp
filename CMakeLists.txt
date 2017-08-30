cmake_minimum_required(VERSION 2.6)
project(lisp)

add_compile_options()
SET( CMAKE_CXX_FLAGS  "-g -Wall -pedantic -O0")

include_directories(include src)

set(SOURCES
        include/lisp.h
        include/repl.h
        src/lisp.c
        src/repl.c
        src/main.c
        )

add_executable(lisp ${SOURCES})