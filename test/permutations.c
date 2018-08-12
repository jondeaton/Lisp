#include <permutations.h>

static int ith_false(const bool booleans[], size_t len, int i);

void nth_permutation(const char *string, int n, char *perm) {
  size_t len = strlen(string);
  bool *used = calloc(sizeof(bool) * len, 1);

  for (int i = (int) len - 1; i >= 0; --i) {
    int digit = n / factorial(i);

    // Get the "digit"'th unused character
    int index = ith_false(used, len, digit);
    used[index] = true;
    perm[i] = string[index];

    // reduce permutation number by one digit
    n = n % factorial(i);
  }
  perm[len] = '\0'; // null terminate permutation
  free(used);
}

int factorial(int n) {
  int f = 1;
  for (int i = 2; i <= n; ++i)
    f *= i;
  return f;
}

static int ith_false(const bool booleans[], size_t len, int i) {
  for (int j = 0; j < (int) len; ++j) {
    if (booleans[j]) continue;
    if (i == 0) return j;
    i--;
  }
  return -1;
}
