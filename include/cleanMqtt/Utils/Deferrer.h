#ifndef INCLUDE_CLEANMQTT_UTILS_DEFERRER_H
#define INCLUDE_CLEANMQTT_UTILS_DEFERRER_H

#include "cleanMqtt/Utils/Event.h"
#include "cleanMqtt/GlobalMacros.h"
#include <queue>
#include <mutex>

namespace cleanMqtt
{

#define deferEvent(deferrer, event, ...)\
	deferrer.defer([&](){event(__VA_ARGS__);})\

	namespace events
	{
		class Deferrer
		{
		public:
			void defer(const std::function<void()>& event) noexcept;
			void invokeEvents() noexcept;
			void clear() noexcept;

			void operator()() noexcept
			{
				invokeEvents();
			}

		private:
			std::queue<std::function<void()>> m_events;
			std::mutex m_mutex;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_UTILS_DEFERRER_H 
