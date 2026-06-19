// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MEMORY_ALLOCATORCONTEXT_H
#define INCLUDE_KMMQTT_MEMORY_ALLOCATORCONTEXT_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/Interfaces/IAllocator.h"

namespace kmMqtt
{
	/**
	 * @brief Allocator context for kmNew/kmDelete helpers.
	 *
	 * If no custom allocator has been assigned, the SDK default allocator is used.
	 */
	PUBLIC_API IAllocator& getAllocator() noexcept;

	/**
	 * @brief Set allocator (nullable).
	 *
	 * Passing nullptr resets the context to use the SDK default allocator.
	 */
	PUBLIC_API void setAllocator(IAllocator* allocator) noexcept;
}

#endif //INCLUDE_KMMQTT_MEMORY_ALLOCATORCONTEXT_H
