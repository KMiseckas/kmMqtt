// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "MqttBenchmarkSupport.h"

#include <benchmark/benchmark.h>

using namespace kmMqtt;

static void BM_MqttConnection_DefaultAllocator(benchmark::State& state)
{
	setAllocator(nullptr);

	for (auto _ : state)
	{
		try
		{
			const auto result = benchmarkSupport::runDeterministicConnectionScenario();
			benchmark::DoNotOptimize(result);

			if (result != benchmarkSupport::ScenarioError::None)
			{
				state.SkipWithError(benchmarkSupport::toString(result));
				break;
			}
		}
		catch (const std::bad_alloc&)
		{
			state.SkipWithError("default allocator threw bad_alloc");
			break;
		}
	}

	setAllocator(nullptr);
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MqttConnection_DefaultAllocator);
