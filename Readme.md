# Lisp Interpreter from C

This repository contains a Lisp interpreter written from scratch in pure C (c99).
The interpreter can be used to run Lisp programs saved in files, or from an
interactive shell. This Lisp interpreter showcases many features including
variable and function declaration, arithmetic operations,
first class/lambda functions, closures, currying, recursive functions,
a mutable global interpreter environment, dynamic scoping, memory allocation
and deterministic memory management without garbage collection.

Although the interpreter and all of it's libraries are written in C, performance
benchmarking and some unit testing are done using
[Google Benchmark](https://github.com/google/benchmark "Google Benchmark")
and
[Google Test](https://github.com/google/googletest "Google Test"),
respectively, which are both C++ libraries. If you want to build
the performance and unit testing binaries, you will need a C++ compiler.

#### Example

The Lisp interpreter comes with a global environment which can be used to set values and then recal them later

    > (set 'x 6)
    6
    > (set 'y 7)
    7
    > (* x y)
    42


This interpreter also supports the creation of closures from lambda functions with variable capture at declaraion time.

    > (set 'make-adder 
        (lambda (x) 
          (lambda (n) (+ x n))))
    <closure:(x), 2 vars captured>
    > (set 'add-2 (make-adder 2))
    <closure:(n), 2 vars captured>
    > (add-2 40)
    42

In this example the `make-adder` function returns a closure with the value of `n` captured within the closure.
Recursive functions may declared, such as this recursive definition of the factorial function:

    > (set 'factorial 
        (lambda (n)
          (cond
            ((= n 0) 1)
            (t       (* n (factorial (- n 1)))))))
    > (factorial 6)
    720

recursion may even be accomplished with anonymous functions via the Y-Combinator

    > (set 'Y
       (lambda (r)
        ((lambda (f) (f f))
         (lambda (f) (r (lambda (x) ((f f) x)))))))
    > (set 'F
       (lambda (g)
        (lambda (n)
         (cond 
          ((= n 0) 1)
          (t       (* n (g (- n 1))))))))
    > ((Y F) 6)
    720


## Usage
If you would like to run this Lisp interpreter from the command line, you will have to
build it from source using the CMake build system. To do so, you will need to have a C99 compiler,
and the CMake build system. To build, issue the following two commands from with in the top-level directory

    cmake .

    make

To run the interactive shell (REPL) use the `lisp` executable.
To have matching parentheses hilighting, run `echo "set blink-matching-paren on" >> ~/.inputrc`.
You can also run a Lisp script by adding it as an argument.

   `./lisp my-program.lisp`

## Dependencies
1. C99
2. Readline
    - Ubuntu: `sudo apt-get install libreadline-dev`
    - MacOS: `brew install readline`

## Testing and Benchmarking
If you would like to check out my handiwork, a testing framework for the interpreter
is also included and can be run with the `test-lisp` executable.

For performance benchmarking, we use the
[Google Benchmark library](https://github.com/google/benchmark "Google Benchmark").
To use this part of the repository, you will need a C++ compiler (as this library
is written in C++) and have installed the library as instructed.

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
- ~~Fix the help/version printout~~
- ~~Use tail recursion instead of loops where possible~~
- ~~Fix bug with `car` and `cdr` of empty list, or end of list~~
- ~~Enable testing framework to report which test is failing~~
- ~~Fix self-referential environment setting~~
- ~~Fix variable capture of overloaded parameter names~~
- ~~Fix the way that unused variables are handled~~
  - ~~`#define UNUSED __attribute__ ((unused))`~~
- ~~Change the error logger to use `snptintf` instead of `sprintf`~~
- ~~Rename the garbage collector to memory manager~~
- ~~Nest structs inside one another where possible (GC, allocated CList)~~
- ~~Use CVector instead of CList~~
- ~~Y (U NO WORK) combinator tests~~
- ~~Fix the way that errors are reported (no `return LOG_ERROR(...)`)~~
- ~~Use flexible array member in lisp objects~~
- Performance benchmarking
- Pass full interpreter to primitives
- Lexical scoping
- Allow circular references by keeping track of freed pointers
- Use a single truth and nil tuple
- `defmacro`
- Avoid deleting large data structures during set with over-write
- Garbage Collector
- Lisp standard library
- Dot notation / Variadic functions
- Strings
