// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H
#define INCLUDE_KMMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H

#include "kmMqtt/Mqtt/State/SessionState/MessageContainerData.h"
#include <list>
#include <unordered_map>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Container for managing MQTT messages with efficient lookup and ordering.
		 * 
		 * This class provides a dual data structure approach using both a list for maintaining
		 * message order and an unordered_map for O(1) packet ID lookups. It is designed to
		 * efficiently manage MQTT messages during session state handling.
		 */
		class MessageContainer
		{
			using MsgIter = std::list<MessageContainerData>::iterator;
			using ConstMsgIter = std::list<MessageContainerData>::const_iterator;

		public:
			/**
			 * @brief Adds a new message to the end of the container.
			 * 
			 * @param msg The message data to be added (moved into the container).
			 * @return Iterator pointing to the newly added message in the list.
			 */
			const MessageContainer::MsgIter push(MessageContainerData&& msg) noexcept;

			/**
			 * @brief Removes a message from the container by its packet ID.
			 * 
			 * @param packetId The unique identifier of the packet to remove.
			 */
			void erase(const std::uint16_t packetId) noexcept;

			/**
			 * @brief Checks if a message with the given packet ID exists in the container.
			 * 
			 * @param packetId The unique identifier of the packet to check.
			 * @return true if the packet ID exists in the container, false otherwise.
			 */
			bool contains(const std::int16_t packetId) const noexcept;

			/**
			 * @brief Retrieves an iterator to the message with the specified packet ID.
			 * 
			 * @param packetId The unique identifier of the packet to retrieve.
			 * @return Pointer to the iterator if found, nullptr otherwise.
			 */
			const MessageContainer::MsgIter* get(const std::uint16_t packetId) noexcept;

			/**
			 * @brief Returns an iterator to the beginning of the message list.
			 * @return Iterator to the first message.
			 */
			MsgIter begin() noexcept { return m_messageList.begin(); }

			/**
			 * @brief Returns an iterator to the end of the message list.
			 * @return Iterator to one past the last message.
			 */
			MsgIter end() noexcept { return m_messageList.end(); }

			/**
			 * @brief Returns a const iterator to the beginning of the message list.
			 * @return Const iterator to the first message.
			 */
			ConstMsgIter begin() const noexcept { return m_messageList.begin(); }

			/**
			 * @brief Returns a const iterator to the end of the message list.
			 * @return Const iterator to one past the last message.
			 */
			ConstMsgIter end() const noexcept { return m_messageList.end(); }

			/**
			 * @brief Returns a const iterator to the beginning of the message list.
			 * @return Const iterator to the first message.
			 */
			ConstMsgIter cbegin() const noexcept { return m_messageList.cbegin(); }

			/**
			 * @brief Returns a const iterator to the end of the message list.
			 * @return Const iterator to one past the last message.
			 */
			ConstMsgIter cend() const noexcept { return m_messageList.cend(); }

			/**
			 * @brief Moves the message with the specified packet ID to the end of the list.
			 * 
			 * @param packetId The unique identifier of the packet to move.
			 * This operation maintains the packet ID mapping and is useful for
			 * implementing message retry or prioritization logic. If the packet ID
			 * is not found, this operation has no effect. This operation is noexcept.
			 */
			void moveToEnd(const std::uint16_t packetId) noexcept;

			/**
			 * @brief Removes all messages from the container.
			 * This operation is noexcept and clears both the list and the map.
			 */
			void clear() noexcept;

			/**
			 * @brief Returns the number of messages in the container.
			 * @return The number of messages currently stored.
			 */
			std::size_t size() const noexcept { return m_messageList.size(); }

		private:
			//Ordered list of messages maintaining insertion/move order.
			std::list<MessageContainerData> m_messageList;

			//Map from packet ID to list iterator for O(1) lookups.
			std::unordered_map<std::uint16_t, MsgIter> m_packetIDToMessageMap;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_SESSIONSTATE_MESSAGECONTAINER_H
