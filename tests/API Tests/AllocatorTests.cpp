// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Memory/AllocatorContext.h>
#include <kmMqtt/Memory/AllocatorUtils.h>
#include <kmMqtt/Memory/DefaultAllocator.h>
#include <kmMqtt/Memory/SdkAllocator.h>
#include <kmMqtt/MqttClient.h>

#include <cstdint>
#include <stdexcept>

#include "Environments/TestEnvironment.h"
#include <cstddef>
#include <kmMqtt/Interfaces/IAllocator.h>
#include <kmMqtt/MqttClientOptions.h>

using namespace kmMqtt;
using namespace kmMqtt::mqtt;

namespace
{
	DefaultAllocator defaultAllocator{};

	class CountingAllocator : public IAllocator
	{
	public:
		void* allocate(std::size_t size, std::size_t alignment) override
		{
			allocateCallCount++;
			lastAllocatedSize = size;
			lastAllocatedAlignment = alignment;

			return defaultAllocator.allocate(size, alignment);
		}

		void deallocate(void* ptr, std::size_t size, std::size_t alignment) noexcept override
		{
			deallocateCallCount++;
			lastDeallocatedSize = size;
			lastDeallocatedAlignment = alignment;

			defaultAllocator.deallocate(ptr, size, alignment);
		}

		std::size_t allocateCallCount{ 0U };
		std::size_t deallocateCallCount{ 0U };
		std::size_t lastAllocatedSize{ 0U };
		std::size_t lastAllocatedAlignment{ 0U };
		std::size_t lastDeallocatedSize{ 0U };
		std::size_t lastDeallocatedAlignment{ 0U };
	};

	struct alignas(64) OverAlignedType
	{
		char data[64];
	};

	struct LifetimeType
	{
		explicit LifetimeType(bool& wasDestroyedRef)
			: wasDestroyed{ wasDestroyedRef }
		{
		}

		~LifetimeType()
		{
			wasDestroyed = true;
		}

		bool& wasDestroyed;
	};

	struct ThrowOnConstruction
	{
		ThrowOnConstruction()
		{
			throw std::runtime_error("constructor failed");
		}
	};
}

TEST_SUITE("Allocator Infrastructure Tests")
{

	TEST_CASE("Default allocator allocates zero-size safely")
	{
		void* ptr = defaultAllocator.allocate(0U, alignof(std::max_align_t));
		CHECK(ptr != nullptr);

		defaultAllocator.deallocate(ptr, 0U, alignof(std::max_align_t));
	}

	TEST_CASE("Default allocator supports over-aligned allocation")
	{
		void* ptr = defaultAllocator.allocate(sizeof(OverAlignedType), alignof(OverAlignedType));
		CHECK(ptr != nullptr);
		CHECK((reinterpret_cast<std::uintptr_t>(ptr) % alignof(OverAlignedType)) == 0U);

		defaultAllocator.deallocate(ptr, sizeof(OverAlignedType), alignof(OverAlignedType));
	}

	TEST_CASE("kmNew and kmDelete use allocator and object lifetime")
	{
		CountingAllocator allocator;
		bool wasDestroyed = false;
		LifetimeType* instance = kmNewWith(allocator, LifetimeType, wasDestroyed);

		CHECK(instance != nullptr);
		CHECK(allocator.allocateCallCount == 1U);
		CHECK(allocator.lastAllocatedSize == sizeof(LifetimeType));
		CHECK(allocator.lastAllocatedAlignment == alignof(LifetimeType));

		kmDelete(allocator, instance);
		CHECK(wasDestroyed);
		CHECK(allocator.deallocateCallCount == 1U);
		CHECK(allocator.lastDeallocatedSize == sizeof(LifetimeType));
		CHECK(allocator.lastDeallocatedAlignment == alignof(LifetimeType));
	}

	TEST_CASE("kmNew deallocates memory when constructor throws")
	{
		CountingAllocator allocator;
		setAllocator(&allocator);

		CHECK_THROWS_AS(kmNew(ThrowOnConstruction), std::runtime_error);
		CHECK(allocator.allocateCallCount == 1U);
		CHECK(allocator.deallocateCallCount == 1U);
		CHECK(allocator.lastAllocatedSize == sizeof(ThrowOnConstruction));
		CHECK(allocator.lastDeallocatedSize == sizeof(ThrowOnConstruction));

		setAllocator(nullptr);
	}

	TEST_CASE("kmNew macro supports zero-arg construction path")
	{
		CountingAllocator allocator;
		setAllocator(&allocator);

		CHECK_THROWS_AS(kmNew(ThrowOnConstruction), std::runtime_error);
		CHECK(allocator.allocateCallCount == 1U);
		CHECK(allocator.deallocateCallCount == 1U);

		setAllocator(nullptr);
	}

	TEST_CASE("SdkAllocator uses SDK allocator for std containers")
	{
		CountingAllocator allocator;
		{
			std::vector<int, SdkAllocator<int>> values{ SdkAllocator<int>(&allocator) };
			values.push_back(1);
			values.push_back(2);
			values.push_back(3);
			CHECK(values.size() == 3U);
		}

		CHECK(allocator.allocateCallCount > 0U);
		CHECK(allocator.deallocateCallCount > 0U);
	}

	TEST_CASE("SdkAllocator supports null allocator fallback and equality semantics")
	{
		SdkAllocator<int> nullA;
		SdkAllocator<int> nullB;
		CHECK(nullA == nullB);

		CountingAllocator allocator;
		SdkAllocator<int> intAllocator(&allocator);
		SdkAllocator<float> floatAllocator(&allocator);
		SdkAllocator<float> floatAllocatorOther;

		CHECK(intAllocator == floatAllocator);
		CHECK(intAllocator != floatAllocatorOther);
	}
}
