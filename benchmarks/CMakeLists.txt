cmake_minimum_required (VERSION 3.14)

project ("cleanMqttBenchmarks" LANGUAGES CXX)

file(GLOB_RECURSE benchmarks_source_files "*.cpp")

#Turn off testing in google benchmark to avoid pulling in extra dependencies.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
	benchmark
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.9.4
)
FetchContent_MakeAvailable(benchmark)

add_executable(${PROJECT_NAME} ${benchmarks_source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE cleanMqtt benchmark::benchmark)