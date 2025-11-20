#ifndef INCLUDE_CLEANMQTT_UTILS_DEFERRER_H
#define INCLUDE_CLEANMQTT_UTILS_DEFERRER_H

#include "cleanMqtt/Utils/Event.h"
#include "cleanMqtt/GlobalMacros.h"
#include <queue>
#include <mutex>
#include <type_traits>
#include <memory>

namespace cleanMqtt
{

#define deferEvent(deferrer, lambda)\
	deferrer.defer(lambda)\

#define deferPacketEvent_wMove(deferrer, event, packet, ...)\
	deferrer.defer([&, p = std::move(packet)](){event(__VA_ARGS__, p);})\

#define deferEvent_wCaptures(deferrer, event, captures, ...)\
	deferrer.defer([captures](){event(__VA_ARGS__);})\

	namespace events
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

		class Deferrer
		{
		public:
			template<typename TFunc>
			void defer(TFunc&& event)
			{
				m_events.emplace(std::make_unique<Callable<TFunc>>(std::forward<TFunc>(event)));
			}

			void invokeEvents() noexcept;
			void clear() noexcept;

			void operator()() noexcept
			{
				invokeEvents();
			}

		private:
			std::queue<std::unique_ptr<ICallable>> m_events;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_UTILS_DEFERRER_H 
