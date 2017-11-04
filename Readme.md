# Lisp
Author: Jon Deaton

## Description
This repository contains a Common Lisp interpreter.


## Design choices
- Object structure (`obj`)
    - All Lisp objects will begin with an `enum` which is one of the following: `atom_obj`, `list_obj`, `primitive_obj` indicating whether the object is either an atom, list, or primitive operation, respectively.
    - The contents of the Lisp object are be stored in memory adjacent to the type-indicating `enum` as follows:
        - `atom_obj`: The raw C-string is stored adjacent to the `enum`.
        - `list_obj`: Two pointers to other list object (`obj*`) are stored after the `enum`, and are named `car` and `cdr`, respectively.
        - `primitive_obj`: A function pointer to a primitive operation is stored adjacent to the `enum`.
- The empty list shall be a a list object with two `NULL` pointers in `car` and `cdr`.
- Dynamic scoping
    - All environment variables are stored in a Lisp list of pairs
    - Calls to the `set` primitive will lookup if there is a previously stored value for the specified primitive. If so, that value will be over-written, otherwise a new value will be pushed on to the top of the environment "stack".
- Results of computation will only be copied when they are being set in the environment
- Lambda functions
    - When evaluating an object, the interpreter will check if `caar` of the object is equal to the C-string `lambda`.
    - If it is, when 
- Garbage Collection
    - wef