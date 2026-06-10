// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MEMORY_DEFAULTALLOCATOR_H
#define INCLUDE_KMMQTT_MEMORY_DEFAULTALLOCATOR_H

#include "kmMqtt/Interfaces/IAllocator.h"

namespace kmMqtt
{
	class DefaultAllocator final : public IAllocator
	{
	public:
		void* allocate(std::size_t size, std::size_t alignment) override;
		void deallocate(void* ptr, std::size_t size, std::size_t alignment) noexcept override;
	};
}

#endif //INCLUDE_KMMQTT_MEMORY_DEFAULTALLOCATOR_H
