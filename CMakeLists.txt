cmake_minimum_required(VERSION 2.6)
project(Lisp)

add_compile_options()
SET(CMAKE_CXX_FLAGS  "-g -O0 -Wall -pedantic")

include_directories(include src)

set(SOURCES
        include/lisp.h
        include/repl.h
        include/environment.h
        include/parser.h
        src/lisp.c
        src/repl.c
        src/environment.c
        src/parser.c
        src/main.c)

add_executable(lisp ${SOURCES})