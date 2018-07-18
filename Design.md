# Design documentation

Quite a few design choices were made in the designing of this Lisp interpreter. In this document I summarize these

- Object structure (`obj`)
    - All Lisp objects will begin with an `enum` which is one of the following: `atom_obj`, `list_obj`, `primitive_obj` indicating whether the object is either an atom, list, or primitive operation, respectively.
    - The contents of the Lisp object are be stored in memory adjacent to the type-indicating `enum` as follows:
        - `atom_obj`: The raw C-string is stored adjacent to the `enum`.
        - `list_obj`: Two pointers to other list object (`obj*`) are stored after the `enum`, and are named `car` and `cdr`, respectively.
        - `primitive_obj`: A function pointer to a primitive operation is stored adjacent to the `enum`.
- The empty list, despite being considered an atom type, shall be a a list object (`list_obj`) with two `NULL` pointers in `car` and `cdr`.
- Single environment
    - In a Lisp-1 manner, there is only a single environment that stores both variables and functions.
- Results of computation will only be copied when they are being set in the environment
- Lambda functions
    - When evaluating an object, the interpreter will check if `caar` of the object is equal to the C-string `lambda`.
    - If it is, then the arguments will be evaluated into a new list, bound to the parameters, and prepended onto the current environment
    - The body of the lambda expression will then be evaluated in this augmented environment
- Memory Management
    - Garbage collection in this Lisp interpreter is much easier to implement than it would be to write a generic garbage collector in say C.
    - It is easy to track the lifetimes of objects **not** created during evaluation
        - For instance, parsed objects are not modified during evaluation, and are *copied* into the environment. Therefore, after evaluation, the parsed object can be recursively deleted.
    - Objects created during evaluation have a somewhat more complicated lifetime, and this are tracked as such:
        - Objects allocated during evaluation (such as in `cons`, or creation of closures) are added to a CVector of allocated objects
        - After each expression evaluation (excluding *recursive* calls to `eval`), the entire vector of allocated object pointers is disposed of.
        - Objects with a lifetime longer than the single evaluation has been copied into the environment at the conclusion of `eval`.
        - Closures create an interesting challenge: lambda expressions are promoted to closure status during evaluation. Thus, closures are counted as dynamically allocated and are added to the vector of blocks to be freed.
- Error reporting
    - Basic stack traces are provided for inappropriate Lisp code.