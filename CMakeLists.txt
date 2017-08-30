cmake_minimum_required(VERSION 2.6)
project(lisp)

add_compile_options()
SET( CMAKE_CXX_FLAGS  "-g -Wall -pedantic -O0")

include_directories(include src)

set(SOURCES
        src/main.c
        src/lisp.c
        include/lisp.h)

add_executable(lisp ${SOURCES})