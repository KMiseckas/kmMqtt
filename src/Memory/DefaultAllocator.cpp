// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Memory/DefaultAllocator.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <new>

namespace kmMqtt
{
	namespace
	{
		std::size_t normalizeAlignment(std::size_t alignment) noexcept
		{
			if (alignment == 0U)
			{
				alignment = alignof(std::max_align_t);
			}

			return std::max(alignment, alignof(void*));
		}
	}

	void* DefaultAllocator::allocate(std::size_t size, std::size_t alignment)
	{
		const std::size_t normalizedAlignment = normalizeAlignment(alignment);
		const std::size_t normalizedSize = size == 0U ? 1U : size;
		const std::size_t overhead = sizeof(void*) + (normalizedAlignment - 1U);

		if (normalizedSize > std::numeric_limits<std::size_t>::max() - overhead)
		{
			throw std::bad_alloc();
		}

		void* const rawPtr = ::operator new(normalizedSize + overhead);
		std::uintptr_t alignedAddress = reinterpret_cast<std::uintptr_t>(rawPtr) + sizeof(void*);
		const std::size_t remainder = alignedAddress % normalizedAlignment;

		if (remainder != 0U)
		{
			alignedAddress += normalizedAlignment - remainder;
		}

		void* const alignedPtr = reinterpret_cast<void*>(alignedAddress);
		reinterpret_cast<void**>(alignedPtr)[-1] = rawPtr;

		return alignedPtr;
	}

	void DefaultAllocator::deallocate(void* ptr, std::size_t /*size*/, std::size_t /*alignment*/) noexcept
	{
		if (ptr == nullptr)
		{
			return;
		}

		void* const rawPtr = reinterpret_cast<void**>(ptr)[-1];
		::operator delete(rawPtr);
	}
}
