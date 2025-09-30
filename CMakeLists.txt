cmake_minimum_required(VERSION 3.14)
project(AVLBenchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

include(FetchContent)

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark was no found in system, it will be loaded from FetchContent.")

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

# Целевая объект для запуска бенчмарков
add_custom_target(run_benchmarks
    COMMAND ${PROJ_NAME}
        --benchmark_out=../plots/results.json
        --benchmark_out_format=json
    DEPENDS ${PROJ_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running benchmarks..."
)