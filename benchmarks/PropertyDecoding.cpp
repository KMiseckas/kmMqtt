#include <benchmark/benchmark.h>
#include <cleanMqtt/Mqtt/Packets/Properties.h>
#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cleanMqtt/ByteBuffer.h>
#include <vector>
#include <memory>

using namespace cleanMqtt;
using namespace cleanMqtt::mqtt;

static void BM_EncodeProperties_WithSmallData(benchmark::State& state)
{
    Properties properties;
    properties.tryAddProperty(PropertyType::CONTENT_TYPE, UTF8String("example"));
    properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair("key", "value"));
    properties.tryAddProperty(PropertyType::AUTHENTICATION_DATA, BinaryData(1, reinterpret_cast<const std::uint8_t*>("A")));

    for (auto _ : state) 
    {
        ByteBuffer buffer{ properties.encodingSize()};
        properties.encode(buffer);
        benchmark::DoNotOptimize(buffer);
    }
}

static void BM_EncodeProperties_WithLargeData(benchmark::State& state)
{
    Properties properties;
    properties.tryAddProperty(PropertyType::CORRELATION_DATA, BinaryData(50, reinterpret_cast<const std::uint8_t*>("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")));
    properties.tryAddProperty(PropertyType::CONTENT_TYPE, UTF8String("example"));
    properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair("key", "value"));
    properties.tryAddProperty(PropertyType::AUTHENTICATION_DATA, BinaryData(1, reinterpret_cast<const std::uint8_t*>("A")));

    for (auto _ : state)
    {
        ByteBuffer buffer{ properties.encodingSize() };
        properties.encode(buffer);
        benchmark::DoNotOptimize(buffer);
    }
}

static void BM_DecodeProperties_WithLargeData(benchmark::State& state)
{
    Properties properties;
    properties.tryAddProperty(PropertyType::CORRELATION_DATA, BinaryData(50, reinterpret_cast<const std::uint8_t*>("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")));
    properties.tryAddProperty(PropertyType::CONTENT_TYPE, UTF8String("example"));
    properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair("key", "value"));
    properties.tryAddProperty(PropertyType::AUTHENTICATION_DATA, BinaryData(1, reinterpret_cast<const std::uint8_t*>("A")));

    ByteBuffer buffer{ properties.encodingSize() };
    properties.encode(buffer);

    for (auto _ : state)
    {
        Properties decodedProperties;
        ByteBuffer decodeBuffer(buffer);
        auto result{ decodedProperties.decode(decodeBuffer) };
        benchmark::DoNotOptimize(result);
    }
}

static void BM_DecodeProperties_WithSmallData(benchmark::State& state)
{
    Properties properties;
    properties.tryAddProperty(PropertyType::CONTENT_TYPE, UTF8String("example"));
    properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair("key", "value"));
    properties.tryAddProperty(PropertyType::AUTHENTICATION_DATA, BinaryData(1, reinterpret_cast<const std::uint8_t*>("A")));

	ByteBuffer buffer{ properties.encodingSize() };
    properties.encode(buffer);

    for (auto _ : state) 
    {
        Properties decodedProperties;
        ByteBuffer decodeBuffer(buffer);
        auto result{ decodedProperties.decode(decodeBuffer) };
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_EncodeProperties_WithSmallData)->Arg(0)->Arg(1)->Arg(2)->Arg(3);
BENCHMARK(BM_EncodeProperties_WithLargeData)->Arg(0)->Arg(1)->Arg(2)->Arg(3);
BENCHMARK(BM_DecodeProperties_WithSmallData)->Arg(0)->Arg(1)->Arg(2)->Arg(3);
BENCHMARK(BM_DecodeProperties_WithLargeData)->Arg(0)->Arg(1)->Arg(2)->Arg(3);
