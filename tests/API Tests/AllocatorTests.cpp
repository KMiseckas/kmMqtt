// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Memory/AllocatorContext.h>
#include <kmMqtt/Memory/AllocatorUtils.h>
#include <kmMqtt/Memory/DefaultAllocator.h>
#include <kmMqtt/Memory/StdAllocator.h>
#include <kmMqtt/MqttClient.h>
#include <kmMqtt/STL/KmContainers.h>

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

		LifetimeType* instance = kmNewAllocArgs(allocator, LifetimeType, wasDestroyed);

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

	TEST_CASE("StdAllocator can be used explicitly with standard containers")
	{
		CountingAllocator allocator;
		{
			kmMqtt::kmStd::vector<int, StdAllocator<int>> values{ StdAllocator<int>(&allocator) };
			values.push_back(1);
			values.push_back(2);
			values.push_back(3);
			CHECK(values.size() == 3U);
		}

		CHECK(allocator.allocateCallCount > 0U);
		CHECK(allocator.deallocateCallCount > 0U);
	}

	TEST_CASE("kmStd container aliases use assigned allocator by default")
	{
		CountingAllocator allocator;
		setAllocator(&allocator);

		{
			kmMqtt::kmStd::string text(128U, 'x');

			kmMqtt::kmStd::vector<int> vectorValues;
			vectorValues.push_back(1);
			vectorValues.push_back(2);

			kmMqtt::kmStd::deque<int> dequeValues;
			dequeValues.push_back(3);

			kmMqtt::kmStd::list<int> listValues;
			listValues.push_back(4);

			kmMqtt::kmStd::map<int, kmMqtt::kmStd::string> mapValues;
			mapValues.emplace(5, "five");

			kmMqtt::kmStd::multimap<int, kmMqtt::kmStd::string> multimapValues;
			multimapValues.emplace(6, "six");

			kmMqtt::kmStd::unordered_map<int, int> unorderedMapValues;
			unorderedMapValues.emplace(7, 7);

			kmMqtt::kmStd::unordered_multimap<int, int> unorderedMultimapValues;
			unorderedMultimapValues.emplace(8, 8);

			kmMqtt::kmStd::set<int> setValues;
			setValues.insert(9);

			kmMqtt::kmStd::multiset<int> multisetValues;
			multisetValues.insert(10);

			kmMqtt::kmStd::unordered_set<int> unorderedSetValues;
			unorderedSetValues.insert(11);

			kmMqtt::kmStd::unordered_multiset<int> unorderedMultisetValues;
			unorderedMultisetValues.insert(12);

			kmMqtt::kmStd::queue<int> queueValues;
			queueValues.push(13);

			kmMqtt::kmStd::stack<int> stackValues;
			stackValues.push(14);

			kmMqtt::kmStd::priority_queue<int> priorityQueueValues;
			priorityQueueValues.push(15);

			kmMqtt::kmStd::array<int, 2U> arrayValues{ { 16, 17 } };

			CHECK(text.size() == 128U);
			CHECK(vectorValues.size() == 2U);
			CHECK(dequeValues.size() == 1U);
			CHECK(listValues.size() == 1U);
			CHECK(mapValues.size() == 1U);
			CHECK(multimapValues.size() == 1U);
			CHECK(unorderedMapValues.size() == 1U);
			CHECK(unorderedMultimapValues.size() == 1U);
			CHECK(setValues.size() == 1U);
			CHECK(multisetValues.size() == 1U);
			CHECK(unorderedSetValues.size() == 1U);
			CHECK(unorderedMultisetValues.size() == 1U);
			CHECK(queueValues.size() == 1U);
			CHECK(stackValues.size() == 1U);
			CHECK(priorityQueueValues.size() == 1U);
			CHECK(arrayValues[0] == 16);
		}

		setAllocator(nullptr);

		CHECK(allocator.allocateCallCount > 0U);
		CHECK(allocator.deallocateCallCount > 0U);
	}

	TEST_CASE("kmStd containers keep their construction allocator for destruction")
	{
		CountingAllocator allocator;

		{
			setAllocator(&allocator);
			kmMqtt::kmStd::vector<int> values;
			values.reserve(8U);
			values.push_back(1);

			setAllocator(nullptr);
		}

		CHECK(allocator.allocateCallCount > 0U);
		CHECK(allocator.deallocateCallCount > 0U);
	}

	TEST_CASE("kmStd string supports literal prefix concatenation")
	{
		CountingAllocator allocator;
		setAllocator(&allocator);

		{
			kmMqtt::kmStd::string value(128U, 'x');
			kmMqtt::kmStd::string text = "prefix:" + value;

			CHECK(text.find("prefix:") == 0U);
			CHECK(text.size() == value.size() + 7U);
		}

		setAllocator(nullptr);

		CHECK(allocator.allocateCallCount > 0U);
		CHECK(allocator.deallocateCallCount > 0U);
	}
}
