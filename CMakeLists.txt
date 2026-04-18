cmake_minimum_required(VERSION 3.14)
project(Lisp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_library(readline readline HINTS /opt/homebrew/opt/readline/lib /usr/local/opt/readline/lib REQUIRED)
find_path(READLINE_INCLUDE_DIR readline/readline.h HINTS /opt/homebrew/opt/readline/include /usr/local/opt/readline/include)

set(LISP_SRC
        src/interpreter.c
        src/lisp-objects.c
        src/evaluator.c
        src/primitives.c
        src/closure.c
        src/parser.c
        src/list.c
        src/garbage-collector.c
        src/environment.c
        src/math-lib.c
        src/repl.c
        src/stack-trace.c)

add_executable(lisp ${LISP_SRC} src/main.c)
target_include_directories(lisp PRIVATE include src ${READLINE_INCLUDE_DIR})
target_link_libraries(lisp ${readline})
target_compile_options(lisp PRIVATE -Wall -Wextra -pedantic)
target_compile_definitions(lisp PRIVATE PRELUDE_PATH="${CMAKE_SOURCE_DIR}/prelude.lisp")

set(TEST_SRC
        test/eval-test.c
        test/parse-test.c
        test/test.c
        test/main-test.c)

add_executable(test-lisp ${LISP_SRC} ${TEST_SRC})
target_include_directories(test-lisp PRIVATE include src test ${READLINE_INCLUDE_DIR})
target_link_libraries(test-lisp ${readline})
target_compile_options(test-lisp PRIVATE -Wall -Wextra -pedantic)
target_compile_definitions(test-lisp PRIVATE PRELUDE_PATH="${CMAKE_SOURCE_DIR}/prelude.lisp")
