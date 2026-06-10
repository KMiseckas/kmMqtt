// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Memory/AllocatorContext.h"
#include <kmMqtt/Memory/DefaultAllocator.h>

namespace kmMqtt
{
    namespace
    {
        DefaultAllocator& defaultAllocator()
        {
            static DefaultAllocator allocator;
            return allocator;
        }

        IAllocator*& assignedAllocator()
        {
            static IAllocator* allocator = &defaultAllocator();
            return allocator;
        }
    }

    IAllocator& getAllocator() noexcept
    {
        return *assignedAllocator();
    }

    void setAllocator(IAllocator* allocator) noexcept
    {
        assignedAllocator() = allocator ? allocator : &defaultAllocator();
    }
}
