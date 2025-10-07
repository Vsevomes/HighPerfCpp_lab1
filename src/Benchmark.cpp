#include <benchmark/benchmark.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
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

void save_latencies_to_csv(const std::string& filename,
                           const std::vector<long long>& latencies_ns) {
  std::ofstream out(filename);
  out << "latency_ns\n";
  for (auto ns : latencies_ns)
    out << ns << "\n";
  out.close();
  // std::cout << "Saved " << filename << " (" << latencies_ns.size() << " samples)\n";
}

static void BM_Warmup_Cache(benchmark::State& state) {
  AVLMap tree;
  int N = static_cast<int>(state.range(0));
  for (auto _ : state) {
    for (int i = 0; i < N; i++) {
      tree.upsert(i, "warmup");
    }
  }
  benchmark::DoNotOptimize(tree);
}
BENCHMARK(BM_Warmup_Cache)->Arg(1000);

// ======================================================================= //
// ========================= INSERT (FULL BUILD) ========================= //
// ======================================================================= //

static void BM_Insert_Ascending(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "ascending");
  std::vector<long long> latencies; 

  for (auto _ : state) {
    AVLMap tree;
    latencies.reserve(data.size());
    for (int x : data) {
      auto start = std::chrono::high_resolution_clock::now();
      tree.insert(x, std::to_string(x));
      auto end = std::chrono::high_resolution_clock::now();
      latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/insert_latencies_ascending_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Insert_Ascending)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100);

static void BM_Insert_Random(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  std::vector<long long> latencies;

  for (auto _ : state) {
    AVLMap tree;
    latencies.reserve(data.size());
    for (int x : data) {
      auto start = std::chrono::high_resolution_clock::now();
      tree.insert(x, std::to_string(x));
      auto end = std::chrono::high_resolution_clock::now();
      latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/insert_latencies_random_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Insert_Random)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100);

static void BM_Insert_Balanced(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "balanced");
  std::vector<long long> latencies;

  for (auto _ : state) {
    AVLMap tree;
    latencies.reserve(data.size());
    for (int x : data) {
      auto start = std::chrono::high_resolution_clock::now();
      tree.insert(x, std::to_string(x));
      auto end = std::chrono::high_resolution_clock::now();
      latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/insert_latencies_balanced_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Insert_Balanced)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100);

// ======================================================================= //
// =========================== SINGLE INSERT ============================= //
// ======================================================================= //

static void BM_Insert(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  std::vector<long long> latencies; 

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();
    tree.insert(N, "test");
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    tree.erase(N);
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/insert_latencies_single_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Insert)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100000);

// ======================================================================= //
// ================================ FIND ================================= //
// ======================================================================= //

static void BM_Find(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  std::vector<long long> latencies; 

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();
    tree.find(N - 1);
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/find_latencies_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Find)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100000);

// ======================================================================= //
// ================================ ERASE ================================ //
// ======================================================================= //

static void BM_Erase(benchmark::State& state) {
  const int N = static_cast<int>(state.range(0));
  auto data = generate_data(N, "random");
  AVLMap tree;
  for (int x : data) tree.insert(x, std::to_string(x));

  std::vector<long long> latencies; 

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();
    tree.erase(N - 1);
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    tree.insert(N - 1, "test");
  }

  if (!latencies.empty()) {
    std::string filename = "../plots/erase_latencies_" + std::to_string(N) + ".csv";
    save_latencies_to_csv(filename, latencies);
  }
}
BENCHMARK(BM_Erase)->RangeMultiplier(2)->Range(1024, 16384)->Iterations(100000);
