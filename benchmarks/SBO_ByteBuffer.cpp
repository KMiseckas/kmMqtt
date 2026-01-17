#include <benchmark/benchmark.h>
#include <kmMqtt/ByteBuffer.h>
#include <queue>

using namespace kmMqtt;

static void BM_SBO_ByteBuffer(benchmark::State& state) 
{
    const std::size_t bufferSize = static_cast<std::size_t>(state.range(0));

    for (auto _ : state)
    {
		for (std::size_t i = 0; i < 1000; ++i);
        {
            ByteBuffer buf(bufferSize);
            for (std::size_t i = 0; i < bufferSize; ++i)
            {
                buf += static_cast<std::uint8_t>(i & 0xFF);
            }

            benchmark::DoNotOptimize(buf);
        }
    }

}

BENCHMARK(BM_SBO_ByteBuffer)
->Arg(4)
->Arg(8)
->Arg(16)
->Arg(32)
->Arg(64)
->Arg(128)
->Arg(256)
->Arg(512)
->Arg(1024);


