#ifndef LISP_CMAP_BENCH_HPP
#define LISP_CMAP_BENCH_HPP

#include <benchmark/benchmark.h>

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
                         roberts_hash, NULL, NULL, NULL,
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


#endif //LISP_CMAP_BENCH_HPP
