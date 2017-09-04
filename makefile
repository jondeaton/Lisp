CC=gcc
CFLAGS=-g -O0 -Wall -pedantic -std=gnu99

HEADERS=lisp.h repl.h environment.h parser.h run_lisp.h
SOURCES=lisp.c repl.c environment.c parser.c run_lisp.c main.c

lisp:
	$(CC) $(CFLAGS) -o lisp -I src -I include $(SOURCES) $(HEADERS)
