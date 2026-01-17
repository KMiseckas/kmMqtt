// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_EVENTS_EVENTSERVICE_H
#define INCLUDE_MQTTCLIENT_EVENTS_EVENTSERVICE_H

#include "mqttClient/Events/Event.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <memory>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace events
{
	struct HandleHash;

	static std::size_t m_counter;

	struct Handle
	{
		friend HandleHash;

		Handle() noexcept
		{
			m_index = static_cast<std::size_t>(m_counter);
			m_counter++;
		}

		bool operator==(const Handle& rhs) const noexcept
		{
			return m_index == rhs.m_index;
		}

	private:

		std::size_t m_index;
	};

	struct HandleHash 
	{
		std::size_t operator()(const Handle& handle) const noexcept 
		{
			return std::hash<std::size_t>{}(handle.m_index);
		}
	};

	struct IEventHandler
	{
		IEventHandler() = delete;
		IEventHandler(const Handle& handle)
			: m_handle{handle}
		{

		}

		virtual ~IEventHandler() = default;

		bool equals(const Handle& handle) noexcept
		{
			return m_handle == handle;
		}

	protected:
		const Handle m_handle;
	};

	template<typename TEvent>
	struct EventHandler : IEventHandler
	{
		explicit EventHandler(std::function<void(const TEvent&)> handler, Handle handle)
			: m_handler{ handler }, IEventHandler(handle)
		{
		}

		void operator()(const TEvent& data) const noexcept
		{
			m_handler(data);
		}

	protected:
		std::function<void(const TEvent&)> m_handler;
	};

	template<typename TEvent>
	using EventCallback = std::function<void(const TEvent&)>;

	class EventService
	{
	public:
		EventService(const EventService& other) = delete;
		void operator=(const EventService& other) = delete;

		~EventService()
		{
			for (const auto& handlers : m_eventMap)
			{
				for (const auto* handler : handlers.second)
				{
					delete handler;
				}
			}

			m_eventMap.clear();
			m_handleToEventTypeMap.clear();
		}

		static EventService& Instance()
		{
			static EventService instance;
			return instance;
		}

		template<typename TEvent>
		Handle subscribe(EventCallback<TEvent> callback)
		{
			std::type_index typeId{ std::type_index(typeid(TEvent)) };
			std::vector<IEventHandler*>& handlers = m_eventMap[typeId];

			Handle handle;
			IEventHandler* handler = new EventHandler<TEvent>(callback, handle);

			handlers.push_back(handler);
			return handle;
		}

		template<typename TEvent>
		void unsubscribe(const Handle& handle)
		{
			if (m_handleToEventTypeMap.count(handle) == 0)
			{
				return;
			}

			std::type_index typeId{ m_handleToEventTypeMap[handle] };
			m_handleToEventTypeMap.erase(handle);

			assert(m_eventMap.count(typeId) != 0);

			std::vector<IEventHandler*>& handlers = m_eventMap[typeId];

			if (handlers.empty())
			{
				m_eventMap.erase(typeId);
				return;
			}

			auto iter = std::find_if(handlers.begin(), handlers.end(),
				[&handle](IEventHandler* existingHandler)
				{
					EventHandler<TEvent>* castedHandler = static_cast<EventHandler<TEvent>*>(existingHandler);
					return castedHandler->equals(handle);
				});

			if (iter == handlers.end())
			{
				return;
			}

			IEventHandler* handler = handlers[iter];
			delete handler;
			handlers.erase(iter);
		}

		template<typename TEvent>
		void publish(const TEvent& data)
		{
			std::type_index typeId{ std::type_index(typeid(TEvent)) };

			if (m_eventMap.count(typeId) == 0)
			{
				return;
			}

			for (const auto* handler : m_eventMap[typeId])
			{
				const EventHandler<TEvent>* castedHandler{ static_cast<const EventHandler<TEvent>*>(handler) };
				(*castedHandler)(data);
			}
		}

	protected:
		EventService() = default;

		std::unordered_map<std::type_index, std::vector<IEventHandler*>> m_eventMap;
		std::unordered_map<Handle, std::type_index, HandleHash> m_handleToEventTypeMap;
	};

	template<typename TEvent>
	static Handle subscribe(EventCallback<TEvent> callback)
	{
		return EventService::Instance().subscribe(callback);
	}

	template<typename TEvent>
	static void unsubscribe(const Handle& handle)
	{
		EventService::Instance().unsubscribe<TEvent>(handle);
	}

	template<typename TEvent>
	static void publish(const TEvent& data)
	{
		EventService::Instance().publish<TEvent>(data);
	}
}

#endif //INCLUDE_MQTTCLIENT_EVENTS_EVENTSERVICE_H 