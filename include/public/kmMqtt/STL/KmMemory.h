// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_MEMORY_H
#define INCLUDE_KMMQTT_STL_MEMORY_H

#include "kmMqtt/Memory/StdAllocator.h"
#include "kmMqtt/Memory/AllocatorContext.h"

#include <memory>
#include <utility>

namespace kmMqtt
{
    namespace kmStd
    {
        class KmDeleter
        {
		public:
            template<typename T>
            void operator()(T* ptr) const noexcept
            {
				if (ptr == nullptr)
				{
					return;
				}

				ptr->~T();
				kmMqtt::getAllocator().deallocate(ptr, sizeof(T), alignof(T)); 
            }
        };

        template <typename T> 
        using shared_ptr = ::std::shared_ptr<T>;

		template <typename T> 
        using weak_ptr = ::std::weak_ptr<T>;

		template <typename T> 
        using unique_ptr = ::std::unique_ptr<T, KmDeleter>;

        template<typename T, typename... Args> 
        kmStd::shared_ptr<T> make_shared(Args&&... args)
		{
			return std::allocate_shared<T>(StdAllocator<T>{&kmMqtt::getAllocator()},
													std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        kmStd::unique_ptr<T> make_unique(Args&&... args)
        {
			void* memory = kmMqtt::getAllocator().allocate(sizeof(T), alignof(T));

            try
            {
				T* obj = new (memory) T(std::forward<Args>(args)...);
				return kmStd::unique_ptr<T>(obj);
            }
            catch (...)
            {
				kmMqtt::getAllocator().deallocate(memory, sizeof(T), alignof(T)); 
                throw;
            }
        }
    }
}

#endif //INCLUDE_KMMQTT_STL_MEMORY_H