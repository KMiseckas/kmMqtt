// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/State/SessionState/MessageContainer.h>
#include <assert.h>


namespace kmMqtt
{
	namespace mqtt
	{
		const MessageContainer::MsgIter MessageContainer::push(MessageContainerData&& msg) noexcept
		{
			assert(msg.data.packetID != 0);
			assert(!contains(msg.data.packetID));

			const auto id{ msg.data.packetID };

			m_messageList.push_back(std::move(msg));
			
			auto iter = std::prev(m_messageList.end());
			m_packetIDToMessageMap[id] = iter;

			return iter;
		}

		void MessageContainer::erase(const std::uint16_t packetId) noexcept
		{
			if (!contains(packetId))
			{
				return;
			}

			const auto iter{ m_packetIDToMessageMap[packetId] };
			m_messageList.erase(iter);
			m_packetIDToMessageMap.erase(packetId);
		}

		bool MessageContainer::contains(const std::int16_t packetId) const noexcept
		{
			return m_packetIDToMessageMap.find(packetId) != m_packetIDToMessageMap.end();
		}

		const MessageContainer::MsgIter* MessageContainer::get(const std::uint16_t packetId) noexcept
		{
			if (!contains(packetId))
			{
				return nullptr;
			}

			auto& iter{ m_packetIDToMessageMap[packetId] };
			return &iter;
		}

		void MessageContainer::moveToEnd(const std::uint16_t packetId) noexcept
		{
			auto it = m_packetIDToMessageMap.find(packetId);
			if (it == m_packetIDToMessageMap.end())
			{
				return;
			}

			m_messageList.splice(m_messageList.end(), m_messageList, it->second);

			auto newIter = m_messageList.end();
			--newIter;
			it->second = newIter;
		}

		void MessageContainer::clear() noexcept
		{
			m_messageList.clear();
			m_packetIDToMessageMap.clear();
		}
	}
}
