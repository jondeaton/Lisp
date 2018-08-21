#include <benchmark/benchmark.h>
#include <cmap.h>
#include <permutations.h>
#include <alphabet.h>

#include <cmath>

namespace {

  // Map benchmarks
  static void BM_map_insert(benchmark::State &state) {
    CMap *cm = simple_map(sizeof(int), sizeof(int));
    int k = 23;
    int v = 10;

    for (auto _ : state) {
      cmap_insert(cm, &k, &v);
    }

    cmap_dispose(cm);
  }
  BENCHMARK(BM_map_insert);

  static void BM_map_repeated_insertion(benchmark::State &state) {
    CMap *cm = simple_map(sizeof(int), sizeof(int));
    for (auto _ : state) {

      for (int i = 0; i < state.range(0); i++) {
        cmap_insert(cm, &i, &i);
      }
    }
    cmap_dispose(cm);
  }
  BENCHMARK(BM_map_repeated_insertion)->RangeMultiplier(2)->Range(1 << 8, 1 << 16);

  static void BM_map_insert_delete(benchmark::State &state) {
    CMap *cm = cmap_create(sizeof(int), sizeof(int),
                           roberts_hash, memcmp,
                           NULL, NULL,
                           (unsigned int) state.range(1));

    for (int i = - state.range(0) / 2; i < 0; i++)
      cmap_insert(cm, &i, &i);

    for (auto _ : state) {
      for (int i = 0; i < state.range(0); i++)
        cmap_insert(cm, &i, &i);
      for (int i = 0; i < state.range(0); i++)
        cmap_remove(cm, &i);
    }
  }
  BENCHMARK(BM_map_insert_delete)->Ranges({{1 << 6, 1 << 12}, {1 << 7, 1 << 13}});

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
