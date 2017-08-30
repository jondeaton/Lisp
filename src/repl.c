/*
 * File: repl.c
 * ------------
 * Presents the implementation of the Read-Eval-Print Loop
 * for Lisp
 */

#include "repl.h"
#include "string.h"


obj* read(expression e) {

};


obj* eval(obj* o) {

};

expression print(obj* o) {

};

void repl(){

};



static list* parse(char* input) {
  size_t i = 0;
  int len = strlen(input);
  for (int i = 0; i < len; ++i) {

    if (isWhitespace(input[i])) continue;

    if (inpit[i] == ')') return list;

    if (input[i] == '(') {
      size_t listSize = findListSize(input + i); // num chars until closign paren
      list.car = parse(input + i + 1);
      parse(input + i + listSize + 1, list.cdr);
    } else {
      size_t tokenSize = findTokenSize(input); // this is the size of the next token

      char *copy = malloc(tokenSize);
      strcpy(input, copy, tokenSize);

      list.car = copy;
      list.cdr = parse(input + i + next_token_location + 1);
    }
  }
}

static const char kWhitespace[] = {' ', '\n', '\t', NULL};
static bool isWhiteSpace(char character) {
  size_t i = 0;
  while (kWhitespace[i] != NULL) {
    if (character == kWhitespace[i]) return true;
    i++;
  }
  return false;
}