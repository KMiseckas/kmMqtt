// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "MqttBenchmarkSupport.h"

#include <benchmark/benchmark.h>

using namespace kmMqtt;

static void BM_MqttConnection_PreallocatedAllocator(benchmark::State& state)
{
	benchmarkSupport::PreallocatedAllocator<512U * 1024U> allocator{};

	for (auto _ : state)
	{
		allocator.reset();
		setAllocator(&allocator);

		try
		{
			const auto result = benchmarkSupport::runDeterministicConnectionScenario();
			benchmark::DoNotOptimize(result);

			setAllocator(nullptr);

			if (allocator.failed())
			{
				state.SkipWithError("preallocated arena exhausted");
				break;
			}

			if (result != benchmarkSupport::ScenarioError::None)
			{
				state.SkipWithError(benchmarkSupport::toString(result));
				break;
			}
		}
		catch (const std::bad_alloc&)
		{
			setAllocator(nullptr);
			state.SkipWithError("preallocated arena exhausted");
			break;
		}
	}

	setAllocator(nullptr);
	state.counters["arena_bytes"] = static_cast<double>(allocator.capacityBytes());
	state.counters["peak_bytes"] = static_cast<double>(allocator.peakBytesUsed());
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MqttConnection_PreallocatedAllocator);
