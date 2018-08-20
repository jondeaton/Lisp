#include <cmap-test.h>
#include <stdio.h>

#define unused __attribute__ ((unused))

int main (int argc unused, char* argv[] unused) {

#if defined(DEBUG)
  printf("Testing permutations... ");
  bool permutations_working = permutation_correctness_test();
  printf("%s\n", permutations_working ? "success" : "failure");
#endif

  cmap_correctness_test();

  return 0;
}
