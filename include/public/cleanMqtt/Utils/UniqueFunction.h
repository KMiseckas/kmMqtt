#ifndef INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
#define INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Utils/TemplateUtils.h>
#include <type_traits>
#include <memory>
#include <cstddef>

namespace cleanMqtt
{
    /**
    * @brief A move-only type-erased callable wrapper with small buffer optimization.
    *
    * UniqueFunction allows storing and invoking any callable object (such as lambdas, functors, or function pointers)
    * with a void() signature. It is move-only and cannot be copied. The implementation allows the use of small buffer 
    * optimization (SBO) when ENABLE_UNQIUEFUNCTION_SBO is defined.
    *
    * Memory management:
    * - If defined ENABLE_UNQIUEFUNCTION_SBO then small callables (≤ UNIQUEFUNCTION_SBO_MAX_SIZE bytes) are stored in an internal buffer.
    * - Larger callables are stored on the heap.
    * - Move operations preserve the storage strategy of the source object.
    * 
    * Exception guarantee: The call operator provides strong noexcept guarantee.
    *
    * Example usage:
    * @code
    * cleanMqtt::UniqueFunction f = [](){ std::cout << "Hello!"; };
    * f(); //Invokes the stored callable
    * @endcode
    *
    * @note Only callables with a void() signature are supported.
    * @note Copying is disabled, only move operations are allowed.
    */
	class PUBLIC_API UniqueFunction
	{
#ifdef ENABLE_UNIQUEFUNCTION_SBO
#if UNIQUEFUNCTION_SBO_MAX_SIZE <= 0
#define UNIQUEFUNCTION_SBO_MAX_SIZE 32
#endif
		using Storage = std::aligned_storage_t<UNIQUEFUNCTION_SBO_MAX_SIZE, alignof(std::max_align_t)>;
#endif

		/**
		 * @brief Abstract base class for type-erased callable objects.
		 */
		struct ICallable
		{
			virtual ~ICallable() {}
			virtual void call() = 0;
#ifdef ENABLE_UNIQUEFUNCTION_SBO
			virtual void move(void* moveTo) noexcept = 0;
#endif
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

			void call() override
			{
				func();
			}

#ifdef ENABLE_UNIQUEFUNCTION_SBO
			void move(void* moveTo) noexcept override
			{
				new (moveTo) Callable<TFunc>(std::move(func));
			}
#endif

			TFunc func;
		};

	public:
		DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(UniqueFunction)

		/**
		 * @brief Constructs UniqueFunction from any callable object.
		 * 
		 * @note Calls a private constructor that decides whether to use small buffer optimization based on the size and alignment of the callable by determining
		 * if it fits within the predefined constraints at compile time (std::integral_constant...)
		 * 
		 * @tparam TFunc The type of the callable object.
		 * @param func The callable object to store.
		 */
		template<typename TFunc>
		UniqueFunction(TFunc&& func) 
			: UniqueFunction(std::forward<TFunc>(func),
				std::integral_constant<bool, sizeof(Callable<std::decay_t<TFunc>>) <= sizeof(Storage) && alignof(Callable<std::decay_t<TFunc>>) <= alignof(Storage)>{})
		{

		}

		UniqueFunction(UniqueFunction&& other) noexcept
		{
#ifdef ENABLE_UNIQUEFUNCTION_SBO
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
#else
			m_callable = other.m_callable;
#endif

			other.m_callable = nullptr;
		}

		UniqueFunction& operator=(UniqueFunction&& other) noexcept
		{
			if (this == &other) return *this;

			destroy(); // Clean up existing callable just in case.

#ifdef ENABLE_UNIQUEFUNCTION_SBO
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
#else
			m_callable = other.m_callable;
#endif

			other.m_callable = nullptr;
			return *this;
		}

		~UniqueFunction()
		{
			destroy();
		}

		void operator()()
		{
			m_callable->call();
		}

	private:

		/**
		 * @brief Constructs UniqueFunction with small buffer optimization (if ENABLE_UNIQUEFUNCTION_SBO is enabled aswell).
		 */
		template<typename TFunc>
		UniqueFunction(TFunc&& func, std::true_type)
		{
			using TCallable = Callable<std::decay_t<TFunc>>;

#ifdef ENABLE_UNIQUEFUNCTION_SBO
			new (&m_buffer) TCallable(std::forward<TFunc>(func));
			m_callable = reinterpret_cast<ICallable*>(&m_buffer);
			m_usesBuffer = true;
#else
			m_callable = new TCallable(std::forward<TFunc>(func));
#endif
		}

		/**
		 * @brief Constructs UniqueFunction without small buffer optimization.
		 */
		template<typename TFunc>
		UniqueFunction(TFunc&& func, std::false_type)
		{
			using TCallable = Callable<std::decay_t<TFunc>>;

#ifdef ENABLE_UNIQUEFUNCTION_SBO
			m_callable = new TCallable(std::forward<TFunc>(func));
			m_usesBuffer = false;
#else
			m_callable = new TCallable(std::forward<TFunc>(func));
#endif
		}

		/**
		 * @brief Destroys the stored callable object.
		 */
		void destroy() noexcept
		{
			if (m_callable != nullptr)
			{
#ifdef ENABLE_UNIQUEFUNCTION_SBO
				if (m_usesBuffer)
				{
					m_callable->~ICallable(); // Explicitly call destructor
				}
				else
				{
					delete m_callable; // Delete heap-allocated callable
				}
#else
				delete m_callable; // Delete heap-allocated callable
#endif
			}
		}

#ifdef ENABLE_UNIQUEFUNCTION_SBO
		bool m_usesBuffer{ true };
		Storage	m_buffer;
#endif

		ICallable* m_callable{ nullptr };
	};
}

#endif //INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
