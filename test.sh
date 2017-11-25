#!/bin/bash
valgrind --leak-check=full --show-reachable=yes --error-limit=no ./test-lisp
