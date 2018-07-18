# Lisp Interpreter from C

This repository contains a Lisp interpreter written from scratch in pure C (c99).
The interpreter can be used to run Lisp programs saved in files, or from an interactive shell.
This Lisp interpreter showcases many features including variable and function declaration, arithmetic operations
first class/lambda functions, closures, recursive functions,
a mutable global interpreter environment, lexical scoping, memory allocation
and automatic/deterministic memory management without garbage collection.

## Usage
If you would like to run this Lisp interpreter from the command line, you will have to
build it from source using the CMake build system. To do so, you will need to have a C99 compile
and CMake installed, then issue the following two commands from with in the top-level directory

    `cmake`

    `make`


To run the REPL (read-eval-print loop) with the `lisp` executable for an interactive prompt.
If you want to have matching parentheses highlighting then you can `echo "set blink-matching-paren on" >> ~/.inputrc`.
Alternatively run a Lisp script by adding it as an argument.

   `./lisp my-program.lisp`

## Dependencies
1. `C99` compiler
2. Readline
    - Ubuntu: `sudo apt-get install libreadline-dev`
    - MacOS: `brew install readline`

## Testing
If you would like to check out my handiwork, a testing framework for the interpreter is also included
and can be run with the `test-lisp` executable.

## Design

To understand in greater detail the design choices which were made in the creation of this interpreter
you may refer to `design.md`.

## To do
The following "to do" list contains many things which I have accomplished in this project as well
as things which I have not yet accomplished. Please cross one off!

- ~~REPL prompt and re-prompt~~
- ~~Correct parsing and un-parsing~~
- ~~`eval` and `apply`~~
- ~~Implement testing framework~~
- ~~Seven primitives~~
- ~~set primitive~~
- ~~basic `lambda` functionality~~
- ~~Error messages and stack trace~~
- ~~Smart indentation in re-prompt'~~
- ~~Use `readline` for interactive prompt~~
- ~~Math library~~
- ~~`env` primitive to print the environment~~
- ~~memory management~~
- ~~closures~~
- ~~Use CList instead of CVector~~
- ~~Lambda with zero arguments~~
- ~~Closure partial application~~
- ~~Empty expression is not invalid~~
- ~~Signal handler in REPL (to exit gracefully with garbage collection)~~
- ~~Abstract Lisp Interpreter and Garbage Collector into structure~~
- ~~Allow for REPL to run from any file descriptor~~
- ~~Proper error reporting on file reading and malloc failure~~
- ~~Specify lisp history file~~
- ~~Verbose logging functionality with CLI flag~~
- ~~Exit on error during program execution~~
- ~~Tests for proper error behavior~~
- ~~Greater than and less than primitives~~
- ~~lambda primitive instead of special case~~
- Fix the help/version printout 
- Use tail recursion instead of loops where possible
- Fix the way that errors are reported (no return LOG_ERROR)
- Fix the way that unused variables are handled
  - `#define UNUSED __attribute__ ((unused))`
- Change the error logger to use `snptintf` instead of `sprintf`
- Rename the garbage collector to memory manager
- Nest structs inside one another where possible (GC, allocated CList)
- Use CVector instead of CList
- Use flexble array member in lisp objects
- Use a single truth and empty list tuple.
- Variadic functions?
- Y combinator tests
- `defmacro`
- Lisp standard library
- Strings
- Dot notation
