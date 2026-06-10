// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <doctest.h>
#include <kmMqtt/Memory/AllocatorContext.h>
#include <kmMqtt/Memory/DefaultAllocator.h>
#include <kmMqtt/STL/KmMemory.h>

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace
{
	class CountingAllocator : public kmMqtt::IAllocator
	{
	public:
		void* allocate(std::size_t size, std::size_t alignment) override
		{
			++allocateCallCount;
			lastAllocatedSize = size;
			lastAllocatedAlignment = alignment;
			return m_defaultAllocator.allocate(size, alignment);
		}

		void deallocate(void* ptr, std::size_t size, std::size_t alignment) noexcept override
		{
			++deallocateCallCount;
			lastDeallocatedSize = size;
			lastDeallocatedAlignment = alignment;
			m_defaultAllocator.deallocate(ptr, size, alignment);
		}

		std::size_t allocateCallCount{ 0U };
		std::size_t deallocateCallCount{ 0U };
		std::size_t lastAllocatedSize{ 0U };
		std::size_t lastAllocatedAlignment{ 0U };
		std::size_t lastDeallocatedSize{ 0U };
		std::size_t lastDeallocatedAlignment{ 0U };

	private:
		kmMqtt::DefaultAllocator m_defaultAllocator{};
	};

	class ScopedAllocatorOverride
	{
	public:
		explicit ScopedAllocatorOverride(kmMqtt::IAllocator& allocator)
		{
			kmMqtt::setAllocator(&allocator);
		}

		~ScopedAllocatorOverride()
		{
			kmMqtt::setAllocator(nullptr);
		}

		ScopedAllocatorOverride(const ScopedAllocatorOverride&) = delete;
		ScopedAllocatorOverride& operator=(const ScopedAllocatorOverride&) = delete;
	};

	struct DestructionTrackedType
	{
		explicit DestructionTrackedType(bool& destroyedRef, int value)
			: destroyed{ destroyedRef }
			, payload{ value }
		{
		}

		~DestructionTrackedType()
		{
			destroyed = true;
		}

		bool& destroyed;
		int payload;
	};

	struct ThrowOnConstruction
	{
		ThrowOnConstruction()
		{
			throw std::runtime_error("constructor failed");
		}
	};
}

TEST_SUITE("KmMemory Tests")
{
	TEST_CASE("KmDeleter returns early for null pointer")
	{
		CountingAllocator allocator;
		ScopedAllocatorOverride allocatorOverride{ allocator };

		int* ptr = nullptr;
		kmMqtt::kmStd::KmDeleter deleter;
		deleter(ptr);

		CHECK(allocator.allocateCallCount == 0U);
		CHECK(allocator.deallocateCallCount == 0U);
	}

	TEST_CASE("KmDeleter destroys object and deallocates memory")
	{
		CountingAllocator allocator;
		ScopedAllocatorOverride allocatorOverride{ allocator };

		bool destroyed = false;
		void* memory = allocator.allocate(sizeof(DestructionTrackedType), alignof(DestructionTrackedType));
		auto* object = new (memory) DestructionTrackedType(destroyed, 42);

		kmMqtt::kmStd::KmDeleter deleter;
		deleter(object);

		CHECK(destroyed);
		CHECK(allocator.deallocateCallCount == 1U);
		CHECK(allocator.lastDeallocatedSize == sizeof(DestructionTrackedType));
		CHECK(allocator.lastDeallocatedAlignment == alignof(DestructionTrackedType));
	}

	TEST_CASE("make_unique allocates constructs and destroys through allocator")
	{
		CountingAllocator allocator;
		ScopedAllocatorOverride allocatorOverride{ allocator };

		bool destroyed = false;
		{
			auto instance = kmMqtt::kmStd::make_unique<DestructionTrackedType>(destroyed, 77);
			REQUIRE(instance != nullptr);
			CHECK(instance->payload == 77);
			CHECK(allocator.allocateCallCount == 1U);
			CHECK(allocator.lastAllocatedSize == sizeof(DestructionTrackedType));
			CHECK(allocator.lastAllocatedAlignment == alignof(DestructionTrackedType));
			CHECK(allocator.deallocateCallCount == 0U);
		}

		CHECK(destroyed);
		CHECK(allocator.deallocateCallCount == 1U);
		CHECK(allocator.lastDeallocatedSize == sizeof(DestructionTrackedType));
		CHECK(allocator.lastDeallocatedAlignment == alignof(DestructionTrackedType));
	}

	TEST_CASE("make_unique deallocates when constructor throws")
	{
		CountingAllocator allocator;
		ScopedAllocatorOverride allocatorOverride{ allocator };

		CHECK_THROWS_AS(kmMqtt::kmStd::make_unique<ThrowOnConstruction>(), std::runtime_error);
		CHECK(allocator.allocateCallCount == 1U);
		CHECK(allocator.deallocateCallCount == 1U);
		CHECK(allocator.lastAllocatedSize == sizeof(ThrowOnConstruction));
		CHECK(allocator.lastDeallocatedSize == sizeof(ThrowOnConstruction));
		CHECK(allocator.lastAllocatedAlignment == alignof(ThrowOnConstruction));
		CHECK(allocator.lastDeallocatedAlignment == alignof(ThrowOnConstruction));
	}

	TEST_CASE("make_shared uses km allocator and supports weak alias")
	{
		CountingAllocator allocator;
		ScopedAllocatorOverride allocatorOverride{ allocator };

		{
			kmMqtt::kmStd::weak_ptr<DestructionTrackedType> weak;
			{
				bool destroyed = false;
				kmMqtt::kmStd::shared_ptr<DestructionTrackedType> shared =
					kmMqtt::kmStd::make_shared<DestructionTrackedType>(destroyed, 91);
				REQUIRE(shared != nullptr);
				CHECK(shared->payload == 91);

				weak = shared;
				CHECK_FALSE(weak.expired());
				CHECK(allocator.allocateCallCount > 0U);
				CHECK(allocator.lastAllocatedAlignment >= alignof(DestructionTrackedType));
			}

			CHECK(weak.expired());
		}
		
		CHECK(allocator.deallocateCallCount > 0U);
	}
}