# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.9.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.9.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jonpdeaton/GitHub/lisp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jonpdeaton/GitHub/lisp

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/Cellar/cmake/3.9.0/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/local/Cellar/cmake/3.9.0/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/jonpdeaton/GitHub/lisp/CMakeFiles /Users/jonpdeaton/GitHub/lisp/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/jonpdeaton/GitHub/lisp/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named test.o

# Build rule for target.
test.o: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test.o
.PHONY : test.o

# fast build rule for target.
test.o/fast:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/build
.PHONY : test.o/fast

#=============================================================================
# Target rules for targets named lisp.o

# Build rule for target.
lisp.o: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 lisp.o
.PHONY : lisp.o

# fast build rule for target.
lisp.o/fast:
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/build
.PHONY : lisp.o/fast

src/environment.o: src/environment.c.o

.PHONY : src/environment.o

# target to build an object file
src/environment.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/environment.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/environment.c.o
.PHONY : src/environment.c.o

src/environment.i: src/environment.c.i

.PHONY : src/environment.i

# target to preprocess a source file
src/environment.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/environment.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/environment.c.i
.PHONY : src/environment.c.i

src/environment.s: src/environment.c.s

.PHONY : src/environment.s

# target to generate assembly for a file
src/environment.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/environment.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/environment.c.s
.PHONY : src/environment.c.s

src/lisp.o: src/lisp.c.o

.PHONY : src/lisp.o

# target to build an object file
src/lisp.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/lisp.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/lisp.c.o
.PHONY : src/lisp.c.o

src/lisp.i: src/lisp.c.i

.PHONY : src/lisp.i

# target to preprocess a source file
src/lisp.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/lisp.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/lisp.c.i
.PHONY : src/lisp.c.i

src/lisp.s: src/lisp.c.s

.PHONY : src/lisp.s

# target to generate assembly for a file
src/lisp.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/lisp.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/lisp.c.s
.PHONY : src/lisp.c.s

src/list.o: src/list.c.o

.PHONY : src/list.o

# target to build an object file
src/list.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/list.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/list.c.o
.PHONY : src/list.c.o

src/list.i: src/list.c.i

.PHONY : src/list.i

# target to preprocess a source file
src/list.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/list.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/list.c.i
.PHONY : src/list.c.i

src/list.s: src/list.c.s

.PHONY : src/list.s

# target to generate assembly for a file
src/list.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/list.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/list.c.s
.PHONY : src/list.c.s

src/main.o: src/main.c.o

.PHONY : src/main.o

# target to build an object file
src/main.c.o:
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/main.c.o
.PHONY : src/main.c.o

src/main.i: src/main.c.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.c.i:
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/main.c.i
.PHONY : src/main.c.i

src/main.s: src/main.c.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.c.s:
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/main.c.s
.PHONY : src/main.c.s

src/parser.o: src/parser.c.o

.PHONY : src/parser.o

# target to build an object file
src/parser.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/parser.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/parser.c.o
.PHONY : src/parser.c.o

src/parser.i: src/parser.c.i

.PHONY : src/parser.i

# target to preprocess a source file
src/parser.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/parser.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/parser.c.i
.PHONY : src/parser.c.i

src/parser.s: src/parser.c.s

.PHONY : src/parser.s

# target to generate assembly for a file
src/parser.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/parser.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/parser.c.s
.PHONY : src/parser.c.s

src/repl.o: src/repl.c.o

.PHONY : src/repl.o

# target to build an object file
src/repl.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/repl.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/repl.c.o
.PHONY : src/repl.c.o

src/repl.i: src/repl.c.i

.PHONY : src/repl.i

# target to preprocess a source file
src/repl.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/repl.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/repl.c.i
.PHONY : src/repl.c.i

src/repl.s: src/repl.c.s

.PHONY : src/repl.s

# target to generate assembly for a file
src/repl.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/repl.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/repl.c.s
.PHONY : src/repl.c.s

src/run_lisp.o: src/run_lisp.c.o

.PHONY : src/run_lisp.o

# target to build an object file
src/run_lisp.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/run_lisp.c.o
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/run_lisp.c.o
.PHONY : src/run_lisp.c.o

src/run_lisp.i: src/run_lisp.c.i

.PHONY : src/run_lisp.i

# target to preprocess a source file
src/run_lisp.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/run_lisp.c.i
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/run_lisp.c.i
.PHONY : src/run_lisp.c.i

src/run_lisp.s: src/run_lisp.c.s

.PHONY : src/run_lisp.s

# target to generate assembly for a file
src/run_lisp.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/src/run_lisp.c.s
	$(MAKE) -f CMakeFiles/lisp.o.dir/build.make CMakeFiles/lisp.o.dir/src/run_lisp.c.s
.PHONY : src/run_lisp.c.s

test/test.o: test/test.c.o

.PHONY : test/test.o

# target to build an object file
test/test.c.o:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/test/test.c.o
.PHONY : test/test.c.o

test/test.i: test/test.c.i

.PHONY : test/test.i

# target to preprocess a source file
test/test.c.i:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/test/test.c.i
.PHONY : test/test.c.i

test/test.s: test/test.c.s

.PHONY : test/test.s

# target to generate assembly for a file
test/test.c.s:
	$(MAKE) -f CMakeFiles/test.o.dir/build.make CMakeFiles/test.o.dir/test/test.c.s
.PHONY : test/test.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... test.o"
	@echo "... lisp.o"
	@echo "... src/environment.o"
	@echo "... src/environment.i"
	@echo "... src/environment.s"
	@echo "... src/lisp.o"
	@echo "... src/lisp.i"
	@echo "... src/lisp.s"
	@echo "... src/list.o"
	@echo "... src/list.i"
	@echo "... src/list.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/parser.o"
	@echo "... src/parser.i"
	@echo "... src/parser.s"
	@echo "... src/repl.o"
	@echo "... src/repl.i"
	@echo "... src/repl.s"
	@echo "... src/run_lisp.o"
	@echo "... src/run_lisp.i"
	@echo "... src/run_lisp.s"
	@echo "... test/test.o"
	@echo "... test/test.i"
	@echo "... test/test.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
