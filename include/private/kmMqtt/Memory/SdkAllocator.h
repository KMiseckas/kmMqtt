// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MEMORY_SDKALLOCATOR_H
#define INCLUDE_KMMQTT_MEMORY_SDKALLOCATOR_H

#include <cstddef>
#include <limits>
#include <new>
#include <type_traits>

namespace kmMqtt
{
	template<class T>
	class SdkAllocator
	{
	public:
		using value_type = T;
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;
		using is_always_equal = std::false_type;

		template<class U>
		struct rebind
		{
			using other = SdkAllocator<U>;
		};

		SdkAllocator() noexcept 
			: m_allocator{&GetDefaultAllocatorInstance()}
		{
		}

		explicit SdkAllocator(IAllocator* allocator) noexcept
			: m_allocator{ allocator }
		{
		}

		template<class U>
		SdkAllocator(const SdkAllocator<U>& other) noexcept
			: m_allocator{ other.getAllocator() }
		{
		}

		T* allocate(std::size_t n)
		{
			if (n > max_size())
			{
				throw std::bad_alloc();
			}

			const std::size_t bytes = n * sizeof(T);
			void* ptr = m_allocator->allocate(bytes, alignof(T));

			if (ptr == nullptr)
			{
				throw std::bad_alloc();
			}

			return static_cast<T*>(ptr);
		}

		void deallocate(T* ptr, std::size_t n) noexcept
		{
			m_allocator->deallocate(ptr, n * sizeof(T), alignof(T));
		}

		std::size_t max_size() const noexcept
		{
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		IAllocator* getAllocator() const noexcept
		{
			return m_allocator;
		}

	private:
		template<class U>
		friend class SdkAllocator;

		IAllocator* m_allocator{ nullptr };
	};

	template<class T, class U>
	bool operator==(const SdkAllocator<T>& lhs, const SdkAllocator<U>& rhs) noexcept
	{
		return lhs.getAllocator() == rhs.getAllocator();
	}

	template<class T, class U>
	bool operator!=(const SdkAllocator<T>& lhs, const SdkAllocator<U>& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

#endif //INCLUDE_KMMQTT_MEMORY_SDKALLOCATOR_H
