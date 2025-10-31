#include <benchmark/benchmark.h>
#include "Functor.h"
#include <cleanMqtt/Utils/UniqueFunction.h>
#include <queue>

using namespace cleanMqtt;

static constexpr std::size_t testOne{ 1 };
static constexpr std::size_t testTwo{ 24 };
static constexpr std::size_t testThree{ 32 };
static constexpr std::size_t testFour{ 100 };

static void BM_SBO_UniqueFunction(benchmark::State& state) 
{
	if (state.range(0) == testOne) //Actually 9 bytes, but alignment makes it 16 bytes.
    {
        for (auto _ : state)
        {
			UniqueFunction func(Functor<testOne>{});
            benchmark::DoNotOptimize(func);
        }
    }
	else if (state.range(0) == testTwo) //Actually 25 bytes, but alignment makes it 32 bytes.
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testTwo>{});
            benchmark::DoNotOptimize(func);
        }
    }
    else if (state.range(0) == testThree) //Actually 33 bytes, but alignment makes it 40 bytes
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testThree>{});
            benchmark::DoNotOptimize(func);
        }
    }
	else if (state.range(0) == testFour) //Actually 101 bytes, but alignment makes it 104 bytes
    {
        for (auto _ : state)
        {
            UniqueFunction func(Functor<testFour>{});
            benchmark::DoNotOptimize(func);
        }
    }
}

// Register benchmarks
BENCHMARK(BM_SBO_UniqueFunction)->Arg(testOne)->Arg(testTwo)->Arg(testThree)->Arg(testFour);


