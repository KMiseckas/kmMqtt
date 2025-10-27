#ifndef INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
#define INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H

#include <cleanMqtt/GlobalMacros.h>
#include <type_traits>
#include <memory>

namespace cleanMqtt
{
	class PUBLIC_API UniqueFunction
	{
		struct ICallable
		{
			virtual ~ICallable() {}
			virtual void call() noexcept = 0;
		};

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

			TFunc func;
		};

		std::unique_ptr<ICallable> callable;

	public:

		template<typename TFunc>
		UniqueFunction(TFunc&& func)
			: callable{ std::make_unique<Callable<std::decay_t<TFunc>>>(std::forward<TFunc>(func)) }
		{
		}

		UniqueFunction(UniqueFunction&&) noexcept = default;
		UniqueFunction& operator=(UniqueFunction&&) noexcept = default;
		UniqueFunction(const UniqueFunction&) = delete;
		UniqueFunction& operator=(const UniqueFunction&) = delete;

		~UniqueFunction() = default;

		void operator()() noexcept
		{
			callable->call();
		}
	};
}

#endif //INCLUDE_CLEAMQTT_UTILS_UNIQUEFUNCTION_H
