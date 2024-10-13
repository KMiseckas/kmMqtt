#pragma once

#include <vector>
#include <mutex>

namespace cleanMqtt
{
	namespace events
	{
		template <typename...Args>
		class Event
		{
		public:
			using LockGuard = std::lock_guard<std::mutex>;
			using Callback = void(*)(Args...);

			void add(const Callback& callback)
			{
				LockGuard lock(m_mutex);

				auto iter = m_callbacks.begin();

				while (iter != m_callbacks.end())
				{
					if (*iter == callback)
					{
						return;
					}
				}

				m_callbacks.emplace_back(callback);
			}

			void remove(const Callback& callback)
			{
				LockGuard lock(m_mutex);

				auto iter = m_callbacks.begin();

				while (iter != m_callbacks.end())
				{
					if (*iter == callback)
					{
						*iter = m_callbacks.back();
						m_callbacks.pop_back();
						break;
					}
				}
			}

			void invoke(Args... args)
			{
				for (const auto& callback : m_callbacks)
				{
					callback(args...);
				}
			}

			void operator()(Args... args) { invoke(args...); }
			void operator+=(const Callback& callback) { add(callback); }
			void operator-=(const Callback& callback) { remove(callback) }

		private:
			std::vector<Callback> m_callbacks;
			std::mutex m_mutex;
		};
	}
}