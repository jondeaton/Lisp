#include "lisp.h"

bool isWhiteSpace(char character);

int main(int argc, char* argv[]) {
  list environment;
  addBuiltins(&environment);
  repl(&environment);
}

list* parse(char* input) {
  size_t i = 0;
  int len = strlen(input);
  for (int i = 0; i < len; ++i) {

    if (isWhitespace(input[i])) continue;

    if (inpit[i] == ')') return list;

    if (input[i] == '(')
    {
      size_t listSize = findListSize(input + i); // num chars until closign paren
      list.car = str2list(input + i +  1);
      list.cdr = new cell();
      str2list(input + i  + listSize + 1, list.cdr);
    }
    else
    {
      size_t tokenSize = findTokenSize(input); // this is the size of the next token

      char* copy = malloc(tokenSize);
      strcpy(input, copy, tokenSize);

      list.car = copy;
      list.cdr = str2list(input + i + next_token_location + 1);
    }
  }
}


void repl(list* env) {
  // todo
}

void addBuiltins(list* env) {
  // todo
}

const char kWhitespace[] = {' ', '\n', '\t', NULL};
bool isWhiteSpace(char character) {
  size_t i = 0;
  while (kWhitespace[i] != NULL) {
    if (character == kWhitespace[i]) return true;
    i++;
  }
  return false;
}