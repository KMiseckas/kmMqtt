#ifndef INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
#define INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H

#include <cleanMqtt/GlobalMacros.h>
#include <type_traits>
#include <memory>

namespace cleanMqtt
{
    /**
    * @brief A move-only type-erased callable wrapper with small buffer optimization.
    *
    * UniqueFunction allows storing and invoking any callable object (such as lambdas, functors, or function pointers)
    * with a void() signature. It is move-only and cannot be copied. The implementation uses small buffer optimization (SBO)
    * to avoid heap allocations for small callable objects.
    *
    * Memory management:
    * - Small callables (≤ 32 bytes) are stored in an internal buffer
    * - Larger callables are stored on the heap
    * - Move operations preserve the storage strategy of the source object
    * 
    * Exception guarantee: The call operator provides strong noexcept guarantee.
    *
    * Example usage:
    * @code
    * cleanMqtt::UniqueFunction f = [](){ std::cout << "Hello!"; };
    * f(); // Invokes the stored callable
    * @endcode
    *
    * @note Only callables with a void() signature are supported.
    * @note Copying is disabled, only move operations are allowed.
    */
	class PUBLIC_API UniqueFunction
	{
		using Storage = std::aligned_storage_t<32U, alignof(std::max_align_t)>;

		/**
		 * @brief Abstract base class for type-erased callable objects.
		 */
		struct ICallable
		{
			virtual ~ICallable() {}
			virtual void call() noexcept = 0;
			virtual void move(void* moveTo) noexcept = 0;
		};

		/**
		 * @brief Concrete implementation of ICallable for a specific callable type.
		 */
		template<typename TFunc>
		struct Callable : ICallable
		{
			explicit Callable(TFunc&& f)
				: func(std::move(f))
			{
			}

			void call() noexcept override
			{
				func();
			}

			void move(void* moveTo) noexcept override
			{
				new (moveTo) Callable<TFunc>(std::move(func));
			}

			TFunc func;
		};

	public:
		DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(UniqueFunction)

		template<typename TFunc>
		UniqueFunction(TFunc&& func)
		{
			using TCallable = Callable<std::decay_t<TFunc>>;
			constexpr bool canUseBuffer{ sizeof(TCallable) <= sizeof(Storage) && alignof(TCallable) <= alignof(Storage) };

            if (canUseBuffer)
			{
				new (&m_buffer) TCallable(std::forward<TFunc>(func));
				m_callable = reinterpret_cast<ICallable*>(&m_buffer);
				m_usesBuffer = true;
			}
			else
			{
				m_callable = new TCallable(std::forward<TFunc>(func));
				m_usesBuffer = false;
			}
		}

		UniqueFunction(UniqueFunction&& other) noexcept
		{
			if (other.m_usesBuffer)
			{
				other.m_callable->move(&m_buffer);
				m_callable = reinterpret_cast<ICallable*>(&m_buffer);
				m_usesBuffer = true;
			}
			else
			{
				m_callable = other.m_callable;
				m_usesBuffer = false;
			}

			other.m_callable = nullptr;
		}

		UniqueFunction& operator=(UniqueFunction&& other) noexcept
		{
			if (this == &other) return *this;

			destroy(); // Clean up existing callable just in case.

			if (other.m_usesBuffer)
			{
				other.m_callable->move(&m_buffer);
				m_callable = reinterpret_cast<ICallable*>(&m_buffer);
				m_usesBuffer = true;
			}
			else
			{
				m_callable = other.m_callable;
				m_usesBuffer = false;
			}

			other.m_callable = nullptr;
			return *this;
		}

		~UniqueFunction()
		{
			destroy();
		}

		void operator()() noexcept
		{
			m_callable->call();
		}

	private:

		/**
		 * @brief Destroys the stored callable object.
		 * This method checks whether the callable is stored in the internal buffer or on the heap.
		 * If it's in the buffer, it explicitly calls the destructor so that resources are properly released.
		 */
		void destroy() noexcept
		{
			if (m_callable != nullptr)
			{
				if (m_usesBuffer)
				{
					m_callable->~ICallable(); // Explicitly call destructor
				}
				else
				{
					delete m_callable; // Delete heap-allocated callable
				}
			}
		}

		Storage	m_buffer;
		ICallable* m_callable{ nullptr };
		bool m_usesBuffer{ true };
	};
}

#endif //INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
