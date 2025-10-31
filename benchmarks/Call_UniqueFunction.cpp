#include <benchmark/benchmark.h>
#include "Functor.h"
#include <cleanMqtt/Utils/UniqueFunction.h>
#include <queue>

using namespace cleanMqtt;

static constexpr std::size_t testOne{ 1 };
static constexpr std::size_t testTwo{ 24 };
static constexpr std::size_t testThree{ 32 };
static constexpr std::size_t testFour{ 100 };

static void BM_Calls_UniqueFunction(benchmark::State& state) 
{
    if (state.range(0) == testOne) 
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testOne>{});
            benchmark::DoNotOptimize(func);
            func();
        }
    }
    else if (state.range(0) == testTwo) 
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testTwo>{});
            benchmark::DoNotOptimize(func);
            func();
        }
    }
    else if (state.range(0) == testThree) 
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testThree>{});
            benchmark::DoNotOptimize(func);
            func();
        }
    }
    else if (state.range(0) == testFour) 
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testFour>{});
            benchmark::DoNotOptimize(func);
            func();
        }
    }
}

// Register benchmarks
BENCHMARK(BM_Calls_UniqueFunction)->Arg(testOne)->Arg(testTwo)->Arg(testThree)->Arg(testFour);
