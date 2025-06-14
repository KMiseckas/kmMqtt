#include "cleanMqtt/Utils/Deferrer.h"

namespace cleanMqtt
{
	namespace events
	{
		void Deferrer::invokeEvents() noexcept
		{
			while (!m_events.empty())
			{
				m_events.front()->call();
				m_events.pop();
			}
		}

		void Deferrer::clear() noexcept
		{
			if (!m_events.empty())
			{
				std::queue<std::unique_ptr<ICallable>> empty;
				m_events.swap(empty);
			}
		}
	}
}
