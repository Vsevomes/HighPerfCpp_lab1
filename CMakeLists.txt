cmake_minimum_required(VERSION 3.14)
project(AVLBenchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-O3 -g -fno-omit-frame-pointer -march=native)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark was not found in system, it will be loaded from FetchContent.")

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main
    )

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark's tests" FORCE)
    FetchContent_MakeAvailable(benchmark)
endif()

include_directories(include)

set(SOURCES
    main.cpp
    src/AVLMap.cpp
    src/Benchmark.cpp
)

set(PROJ_NAME benchmarks)

add_executable(${PROJ_NAME} ${SOURCES})

target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJ_NAME} PRIVATE benchmark::benchmark pthread)

# Настройки компиляции
target_compile_options(${PROJ_NAME} PRIVATE
    -O3
    -march=native
)

# --- Скрипт для запуска бенчмарков с фиксированной частотой ---
set(BENCH_SCRIPT "${CMAKE_BINARY_DIR}/run_benchmarks_with_fixed_freq.sh")

file(WRITE ${BENCH_SCRIPT} "#!/usr/bin/env bash\n")
file(APPEND ${BENCH_SCRIPT} "set -e\n")
file(APPEND ${BENCH_SCRIPT} "echo '--- Disabling Turbo Boost and enabling performance mode ---'\n")
file(APPEND ${BENCH_SCRIPT} "if ! command -v cpupower &>/dev/null; then\n")
file(APPEND ${BENCH_SCRIPT} "  echo 'Error: cpupower not found. Install it with: sudo apt install linux-tools-common linux-tools-$(uname -r)'\n")
file(APPEND ${BENCH_SCRIPT} "  exit 1\n")
file(APPEND ${BENCH_SCRIPT} "fi\n")
file(APPEND ${BENCH_SCRIPT} "sudo cpupower frequency-set -g performance\n")
file(APPEND ${BENCH_SCRIPT} "sudo sh -c 'echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo'\n")

file(APPEND ${BENCH_SCRIPT} "echo '--- Running benchmarks ---'\n")
file(APPEND ${BENCH_SCRIPT} "./${PROJ_NAME} --benchmark_out=../plots/results.json --benchmark_out_format=json\n")

file(APPEND ${BENCH_SCRIPT} "echo '--- Restoring Turbo Boost and powersave governor ---'\n")
file(APPEND ${BENCH_SCRIPT} "sudo cpupower frequency-set -g powersave\n")
file(APPEND ${BENCH_SCRIPT} "sudo sh -c 'echo 0 > /sys/devices/system/cpu/intel_pstate/no_turbo'\n")

file(COPY ${BENCH_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR})
execute_process(COMMAND chmod +x ${BENCH_SCRIPT})

# Цель для запуска бенчмарков с управлением частотой
add_custom_target(run_benchmarks
    COMMAND ${BENCH_SCRIPT}
    DEPENDS ${PROJ_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running benchmarks with fixed CPU frequency (no turbo)..."
)

# --- Скрипт для профилирования и генерации флеймграфов ---
set(FLAME_SCRIPT "${CMAKE_BINARY_DIR}/run_benchmarks_flamegraphs.sh")

file(WRITE ${FLAME_SCRIPT} "
#!/usr/bin/env bash
set -e

BENCH_EXEC=./${PROJ_NAME}
OUT_DIR=../flamegraphs
FLAMEGRAPH_DIR=~/FlameGraph
FREQ=99

mkdir -p \"$OUT_DIR\"

BENCHMARKS=(
  BM_Insert_Ascending
  BM_Insert_Random
  BM_Insert_Balanced
  BM_Insert
  BM_Find
  BM_Erase
)

SIZES=(1024 2048 4096 8192 16384)

for bench in \"\${BENCHMARKS[@]}\"; do
  for size in \"\${SIZES[@]}\"; do
    echo \"Running \$bench (N=\$size)\"

    PERF_DATA=\"\$OUT_DIR/perf_\${bench}_\${size}.data\"
    PERF_SCRIPT=\"\$OUT_DIR/perf_\${bench}_\${size}.folded\"
    SVG=\"\$OUT_DIR/flame_\${bench}_\${size}.svg\"
    SVG_NAME=\"\${bench}_\${size}\"

    sudo perf record -F \$FREQ -g -o \"\$PERF_DATA\" -- \\
      \"\$BENCH_EXEC\" --benchmark_filter=\"\${bench}\"/\"\$size\"

    sudo perf script -i \"\$PERF_DATA\" | \"\$FLAMEGRAPH_DIR/stackcollapse-perf.pl\" > \"\$PERF_SCRIPT\"
    \"\$FLAMEGRAPH_DIR/flamegraph.pl\" --title=\"\$SVG_NAME\" \"\$PERF_SCRIPT\" > \"\$SVG\"

    echo \"Flamegraph saved: \$SVG\"
  done
done

echo \"All flamegraphs generated in \$OUT_DIR\"
")

execute_process(COMMAND chmod +x ${FLAME_SCRIPT})

add_custom_target(run_flamegraphs
    COMMAND bash ${FLAME_SCRIPT}
    DEPENDS ${PROJ_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running benchmarks under perf and generating flamegraphs..."
)
