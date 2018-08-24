#include <benchmark/benchmark.h>

#include <permutations.h>
#include <alphabet.h>
#include <cmap.h>
#include <cmath>

#include <cmap-bench.hpp>

namespace {

  // Permutation benchmarks
  static void BM_permute_next(benchmark::State &state) {
    auto arr = new int[state.range(0)];
    for (int i = 0; i < state.range(0); i++)
      arr[i] = i;

    permuter *p = new_permuter(arr, 4, sizeof(int), cmp_int);
    for (auto _ : state) {
      void *permutation = next_permutation(p);
      benchmark::DoNotOptimize(permutation);
      reset_permuter(p);
    }

    permuter_dispose(p);
    delete[] arr;
  }
  BENCHMARK(BM_permute_next)->Arg(5);

  static void BM_permutation(benchmark::State &state) {
    auto arr = new int[state.range(0)];
    for (int i = 0; i < state.range(0); i++)
      arr[i] = i;

    permuter *p = new_permuter(arr, 4, sizeof(int), cmp_int);
    for (auto _ : state) {
      void *permutation;
      reset_permuter(p);
      for_permutations(p, permutation) {
        benchmark::DoNotOptimize(permutation);
        benchmark::ClobberMemory();
      }
    }

    permuter_dispose(p);
    delete[] arr;
  }
  BENCHMARK(BM_permutation)->DenseRange(3, 5, 1);

  static void BM_permuter_reset(benchmark::State &state) {
    auto arr = new int[state.range(0)];
    for (int i = 0; i < state.range(0); i++)
      arr[i] = -i;

    permuter *p = new_permuter(arr, 4, sizeof(int), cmp_int);

    for (auto _ : state) {
      memcmp(get_permutation(p), arr, sizeof(int) * state.range(0));
      reset_permuter(p);
    }

    permuter_dispose(p);
    delete[] arr;
  }
  BENCHMARK(BM_permuter_reset)->Arg(3);
}


BENCHMARK_MAIN();
