// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MEMORY_ALLOCATORUTILS_H
#define INCLUDE_KMMQTT_MEMORY_ALLOCATORUTILS_H

#include "kmMqtt/Interfaces/IAllocator.h"
#include "kmMqtt/Memory/AllocatorContext.h"

#include <new>
#include <type_traits>
#include <utility>

namespace kmMqtt
{
	namespace
	{
		namespace detail
		{
			template<class...>
			struct FirstType;

			template<class First, class... Rest>
			struct FirstType<First, Rest...>
			{
				using type = First;
			};

			template<class T>
			using DecayT = typename std::decay<T>::type;
		}
	}

	template<class T, class... Args>
	T* kmNewImpl(IAllocator& allocator, Args&&... args)
	{
		void* memory = allocator.allocate(sizeof(T), alignof(T));

		try
		{
			return ::new(memory) T(std::forward<Args>(args)...);
		}
		catch (...)
		{
			allocator.deallocate(memory, sizeof(T), alignof(T));
			throw;
		}
	}

	/**
	 * @brief Allocate/construct using the assigned allocator context (or SDK default).
	 */
	template<class T>
	T* kmNewImpl()
	{
		return kmNewImpl<T>(getAllocator());
	}

	template<class T>
	void kmDeleteImpl(IAllocator& allocator, T* ptr) noexcept
	{
		if (ptr == nullptr)
		{
			return;
		}

		ptr->~T();
		allocator.deallocate(ptr, sizeof(T), alignof(T));
	}

	template<class T>
	void kmDeleteImpl(T* ptr) noexcept
	{
		kmDeleteImpl(getAllocator(), ptr);
	}

	// SFINAE: only enable if the first argument is not an allocator pointer (to avoid ambiguity with the above overloads).
	template<class T, class... Args>
	typename std::enable_if<!std::is_convertible<detail::DecayT<typename detail::FirstType<Args...>::type>*, IAllocator*>::value, T*>::type 
	kmNewImpl(Args&&... args)
	{
		return kmNewImpl<T>(getAllocator(), std::forward<Args>(args)...);
	}
}

#endif //INCLUDE_KMMQTT_MEMORY_ALLOCATORUTILS_H
