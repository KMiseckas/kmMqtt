// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#pragma once

#include "kmMqtt/GlobalMacros.h"
#include <vector>
#include <mutex>
#include <functional>

namespace kmMqtt
{
	namespace events
	{
		template <typename...Args>
		class Event
		{
		public:
			using Callback = std::function<void(Args...)>;

			Event() noexcept = default;
			virtual ~Event() {};

			void add(const Callback& callback)
			{
				LockGuard lock(m_mutex);

				for (auto iter = m_callbacks.begin(); iter != m_callbacks.end(); ++iter)
				{
					if (iter->target_type() == callback.target_type() &&
						iter->template target<void(*)(Args...)>() == callback.template target<void(*)(Args...)>())
					{
						return;
					}
				}

				m_callbacks.emplace_back(callback);
			}

			void remove(const Callback& callback)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				for (auto iter = m_callbacks.begin(); iter != m_callbacks.end(); ++iter)
				{
					if (iter->target_type() == callback.target_type() &&
						iter->template target<void(*)(Args...)>() == callback.template target<void(*)(Args...)>())
					{
						*iter = std::move(m_callbacks.back());
						m_callbacks.pop_back();
						break;
					}
				}
			}

			virtual void invoke(Args... args) noexcept
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				for (const auto& callback : m_callbacks)
				{
					callback(args...);
				}
			}

			void removeAll()
			{
				LockGuard lock(m_mutex);
				m_callbacks.clear();
			}

			void operator()(Args... args) noexcept { invoke(args...); }
			void operator+=(const Callback& callback) { add(callback); }
			void operator-=(const Callback& callback) { remove(callback); }

		private:
			std::vector<Callback> m_callbacks;
			std::mutex m_mutex;
		};
	}
}