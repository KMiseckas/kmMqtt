// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Memory/DefaultAllocator.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <new>
#include <cstring>

namespace kmMqtt
{
	namespace
	{
		bool isPowerOfTwo(std::size_t value) noexcept
		{
			return value != 0U && (value & (value - 1U)) == 0U;
		}

		std::size_t normalizeAlignment(std::size_t alignment)
		{
			if (alignment == 0U)
			{
				alignment = alignof(std::max_align_t);
			}

			alignment = std::max(alignment, alignof(void*));

			if (!isPowerOfTwo(alignment))
			{
				throw std::bad_alloc();
			}

			return alignment;
		}
	} // namespace

	void* DefaultAllocator::allocate(std::size_t size, std::size_t alignment)
	{
		const std::size_t normalizedAlignment = normalizeAlignment(alignment);
		const std::size_t normalizedSize = size == 0U ? 1U : size;

		const std::size_t overhead = sizeof(void*) + normalizedAlignment - 1U;

		if (normalizedSize > std::numeric_limits<std::size_t>::max() - overhead)
		{
			throw std::bad_alloc();
		}

		void* const rawPtr = operator new(normalizedSize + overhead);

		std::uintptr_t rawAddress = reinterpret_cast<std::uintptr_t>(rawPtr);
		std::uintptr_t alignedAddress = rawAddress + sizeof(void*);

		const std::size_t remainder = alignedAddress % normalizedAlignment;
		if (remainder != 0U)
		{
			alignedAddress += (normalizedAlignment - remainder);
		}

		void* const alignedPtr = reinterpret_cast<void*>(alignedAddress);

		void* stashTarget = static_cast<char*>(alignedPtr) - sizeof(void*);
		std::memcpy(stashTarget, &rawPtr, sizeof(void*));

		return alignedPtr;
	}

	void DefaultAllocator::deallocate(void* ptr, std::size_t, std::size_t) noexcept
	{
		if (ptr == nullptr)
		{
			return;
		}

		void* rawPtr = nullptr;
		void* stashTarget = static_cast<char*>(ptr) - sizeof(void*);
		std::memcpy(&rawPtr, stashTarget, sizeof(void*));

		operator delete(rawPtr);
	}
} // namespace kmMqtt
