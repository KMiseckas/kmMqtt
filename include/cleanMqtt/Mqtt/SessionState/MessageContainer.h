#ifndef INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H
#define INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H

#include "cleanMqtt/Mqtt/SessionState/MessageContainerData.h"
#include <list>
#include <unordered_map>
#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		class MessageContainer
		{
			using MsgIter = std::list<MessageContainerData>::iterator;
			using ConstMsgIter = std::list<MessageContainerData>::const_iterator;

		public:
			const MessageContainer::MsgIter push(MessageContainerData&& msg) noexcept;
			void erase(const std::uint16_t packetId) noexcept;
			bool contains(const std::int16_t packetId) const noexcept;
			const MessageContainer::MsgIter* get(const std::uint16_t packetId) noexcept;
			MsgIter begin() noexcept { return m_messageList.begin(); }
			MsgIter end() noexcept { return m_messageList.end(); }
			ConstMsgIter begin() const noexcept { return m_messageList.begin(); }
			ConstMsgIter end() const noexcept { return m_messageList.end(); }
			ConstMsgIter cbegin() const noexcept { return m_messageList.cbegin(); }
			ConstMsgIter cend() const noexcept { return m_messageList.cend(); }
			void moveToEnd(const std::uint16_t packetId) noexcept;
			void clear() noexcept;

		private:
			std::list<MessageContainerData> m_messageList;
			std::unordered_map<std::uint16_t, MsgIter> m_packetIDToMessageMap;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H
