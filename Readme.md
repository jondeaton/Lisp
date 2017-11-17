# Lisp
Author: Jon Deaton

This repository contains an interpreter for Common Lisp implemented in pure C.

## Use
Build with `cmake` like so

`cmake .; make`

Run the REPL (read-eval-print loop) with `lisp` for an interactive prompt, or run a lisp script by adding it as an argument.


## Dependencies
    1. `C99`
    2. GNU Readline (not required)
        - Ubuntu: `sudo apt-get install libedit-dev`
        - Fedora: `su -c "yum install libedit-dev*"`
        - MacOS: Should be installed already but if not `brew install editline`
        - Cygwin: Install from Cygwin Setup
        - Windows: http://gnuwin32.sourceforge.net/packages/readline.htm 

## Design choices
- Object structure (`obj`)
    - All Lisp objects will begin with an `enum` which is one of the following: `atom_obj`, `list_obj`, `primitive_obj` indicating whether the object is either an atom, list, or primitive operation, respectively.
    - The contents of the Lisp object are be stored in memory adjacent to the type-indicating `enum` as follows:
        - `atom_obj`: The raw C-string is stored adjacent to the `enum`.
        - `list_obj`: Two pointers to other list object (`obj*`) are stored after the `enum`, and are named `car` and `cdr`, respectively.
        - `primitive_obj`: A function pointer to a primitive operation is stored adjacent to the `enum`.
- The empty list, despite being considered an atom type, shall be a a list object (`list_obj`) with two `NULL` pointers in `car` and `cdr`.
- Single environment
    - In a Lisp-1 manner, there is only a single environment that stores both variables and functions.
- Dynamic scoping
    - All environment variables are stored in a Lisp list of pairs
    - Calls to the `set` primitive will lookup if there is a previously stored value for the specified primitive. If so, that value will be over-written, otherwise a new value will be pushed on to the top of the environment "stack".
- Results of computation will only be copied when they are being set in the environment
- Lambda functions
    - When evaluating an object, the interpreter will check if `caar` of the object is equal to the C-string `lambda`.
    - If it is, then the arguments will be evaluated into a new list, bound to the parameters, and prepended onto the current environment
    - The body of the lambda expression will then be evaluated in this augmented environment
- Memory Management
    - Uhh... yeah this still needs to be implemented. Just you wait, it's gonna be great.
- Error reporting


## Testing
A testing framework for the interpreter is also included in the `test-lisp` executable.

## To do
- ~~REPL prompt and re-prompt~~
- ~~Correct parsing and un-parsing~~
- ~~`eval` and `apply`~~
- ~~Implement testing framework~~
- ~~Seven primitives~~
- ~~set primitive with dynamic scoping~~
- ~~basic `lambda` functionality~~
- memory management
- Closures
- `defmacro`
- `env` primitive to print the environment
- Error messages and stack trace
- ~~Smart indentation in re-prompt'~~
- ~~Use GNU Readline for interactive prompt~~
- Math library
- [Derive lexical scoping from dynamic scoping](https://stackoverflow.com/questions/29347648/can-dynamic-scoping-implement-lexical-scoping)