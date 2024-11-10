#include "cleanMqtt/Utils/Deferrer.h"

namespace cleanMqtt
{
	namespace events
	{
		void Deferrer::defer(const std::function<void()>& event) noexcept
		{
			m_events.push(event);
		}

		void Deferrer::invokeEvents() noexcept
		{
			while (!m_events.empty())
			{
				m_events.front()();
				m_events.pop();
			}
		}

		void Deferrer::clear() noexcept
		{
			if (!m_events.empty())
			{
				std::queue<std::function<void()>> empty;
				m_events.swap(empty);
			}
		}
	}
}
