#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "AVLMap.h"

// ---------------- Data generation ---------------- //
void generate_balanced(int start, int end, std::vector<int>& data) {
  if (start > end) return;
  int mid = (start + end) / 2;
  data.push_back(mid);
  generate_balanced(start, mid - 1, data);
  generate_balanced(mid + 1, end, data);
}

std::vector<int> generate_data(int n, const std::string& scenario) {
  std::vector<int> data(n);
  std::iota(data.begin(), data.end(), 0);

  if (scenario == "random") {
    std::mt19937 rng(42);
    std::shuffle(data.begin(), data.end(), rng);
  } else if (scenario == "ascending") {
    // already ascending
  } else if (scenario == "balanced") {
    std::vector<int> balanced;
    generate_balanced(0, n - 1, balanced);
    data = balanced;
  }
  return data;
}

// ---------------- Insert ---------------- //
static void BM_Insert_Ascending(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "ascending");

  for (auto _ : state) {
    AVLMap tree;
    for (int x : data) {
      tree.insert(x, std::to_string(x));
    }
  }
}
BENCHMARK(BM_Insert_Ascending)->RangeMultiplier(2)->Range(1, 16384);

static void BM_Insert_Random(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");

  for (auto _ : state) {
    AVLMap tree;
    for (int x : data) {
      tree.insert(x, std::to_string(x));
    }
  }
}
BENCHMARK(BM_Insert_Random)->RangeMultiplier(2)->Range(1, 16384);

static void BM_Insert_Balanced(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "balanced");

  for (auto _ : state) {
    AVLMap tree;
    for (int x : data) {
      tree.insert(x, std::to_string(x));
    }
  }
}
BENCHMARK(BM_Insert_Balanced)->RangeMultiplier(2)->Range(1, 16384);

// ---------------- Insert ---------------- //
static void BM_Insert(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  for (auto _ : state) {
    benchmark::DoNotOptimize(tree.insert(N, "test"));
    tree.erase(N);
  }
}
BENCHMARK(BM_Insert)->RangeMultiplier(2)->Range(1, 16384);

// ---------------- Find ---------------- //
static void BM_Find(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  for (auto _ : state) {
    benchmark::DoNotOptimize(tree.find(N - 1));
  }
}
BENCHMARK(BM_Find)->RangeMultiplier(2)->Range(1, 16384);

// ---------------- Erase ---------------- //
static void BM_Erase(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  for (auto _ : state) {
    benchmark::DoNotOptimize(tree.erase(N - 1));
    tree.insert(N - 1, "test");
  }
}
BENCHMARK(BM_Erase)->RangeMultiplier(2)->Range(1, 16384);
