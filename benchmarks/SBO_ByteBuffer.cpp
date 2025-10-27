#include <benchmark/benchmark.h>
#include <cleanMqtt/ByteBuffer.h>
#include <queue>

using namespace cleanMqtt;

static void BM_SBO_ByteBuffer(benchmark::State& state) 
{
    const std::size_t bufferSize = static_cast<std::size_t>(state.range(0));

    for (auto _ : state)
    {
        ByteBuffer buf(bufferSize);
        for (std::size_t i = 0; i < bufferSize; ++i)
        {
            buf += static_cast<std::uint8_t>(i & 0xFF);
        }

		state.counters["Alloc Bytes"] = sizeof(buf) + buf.size();

        benchmark::DoNotOptimize(buf);
    }

}

BENCHMARK(BM_SBO_ByteBuffer)
->Arg(2)
->Arg(4)
->Arg(8)
->Arg(16)
->Arg(32)
->Arg(64)
->Arg(128)
->Arg(256)
->Arg(512)
->Arg(1024)
->Arg(2048)
->Arg(4096)
->Arg(8192)
->Arg(16384);
