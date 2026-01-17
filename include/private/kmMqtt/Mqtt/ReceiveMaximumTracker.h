// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_PRIVATE_KMMQTT_MQTT_RECEIVEMAXIMUMTRACKER_H
#define INCLUDE_PRIVATE_KMMQTT_MQTT_RECEIVEMAXIMUMTRACKER_H

#include "kmMqtt/GlobalMacros.h"
#include <unordered_set>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Tracks the Receive Maximum allowance for both sending and receiving PUBLISH packets.
		 * Not thread safe itself, but is intended to be used in a thread safe manner by internal system loop.
		 */
		struct ReceiveMaximumTracker
		{
			ReceiveMaximumTracker() = delete;
			ReceiveMaximumTracker(const std::uint32_t receiveMaximumClient, const std::uint32_t receiveMaximumBroker) noexcept
				: m_receiveAllowance{ receiveMaximumClient },
				m_sendAllowance{ receiveMaximumBroker },
				m_maxReceiveAllowance{ receiveMaximumClient },
				m_maxSendAllowance{ receiveMaximumBroker }
			{
			}

			/**
			 * @brief Increments the receive allowance. Used when a PUBLISH packet with the given packet ID has been acknowledged.
			 * This is called when a PUBLISH_ACKNOWLEDGE, PUBLISH_COMPLETE, or PUBLISH_RECEIVED(w/ error code) packet is received for the given packet ID.
			 * Receive Allowance is the amount of publish packets that can be received before hitting the Receive Maximum limit for this client.
			 */
			void incrementReceiveAllowance(const std::uint16_t packetId) noexcept
			{
				if (m_receiveAllowance >= m_maxReceiveAllowance)
				{
					return;
				}

				const auto iter{ m_receivedPublishIDs.find(packetId) };
				if (iter == m_receivedPublishIDs.end())
				{
					return;
				}

				++m_receiveAllowance;
				m_receivedPublishIDs.erase(iter);
			}

			/**
			 * @brief Decrements the receive allowance. Used when a PUBLISH packet with the given packet ID is received.
			 * Returns false if the allowance is already at zero and the packet cannot be accepted.
			 * Receive Allowance is the amount of publish packets that can be received before hitting the Receive Maximum limit for this client.
			 */
			bool decrementReceiveAllowance(const std::uint16_t packetId) noexcept
			{
				if (m_receiveAllowance == 0)
				{
					return false;
				}

				if (m_receivedPublishIDs.find(packetId) != m_receivedPublishIDs.end())
				{
					return true;
				}

				--m_receiveAllowance;
				m_receivedPublishIDs.insert(packetId);
				return true;
			}

			/**
			 * @brief Increments the send allowance. Used when a PUBLISH packet with the given packet ID has been acknowledged.
			 * This is called when a PUBLISH_ACKNOWLEDGE, PUBLISH_COMPLETE, or PUBLISH_RECEIVED(w/ error code) packet is sent for the given packet ID.
			 * Send Allowance is the amount of publish packets that can be sent before hitting the brokers Receive Maximum limit.
			 */
			void incrementSendAllowance(const std::uint16_t packetId) noexcept
			{
				if(m_sendAllowance >= m_maxSendAllowance)
				{
					return;
				}

				const auto iter{ m_sentPublishIDs.find(packetId) };
				if(iter == m_sentPublishIDs.end())
				{
					return;
				}

				++m_sendAllowance;
				m_sentPublishIDs.erase(iter);
			}

			/**
			 * @brief Decrements the send allowance. Used when a PUBLISH packet with the given packet ID is sent.
			 * Returns false if the allowance is already at zero and the packet cannot be sent.
			 * Send Allowance is the amount of publish packets that can be sent before hitting the brokers Receive Maximum limit.
			 */
			bool decrementSendAllowance (const std::uint16_t packetId) noexcept
			{
				if (m_sendAllowance == 0)
				{
					return false;
				}

				if(m_sentPublishIDs.find(packetId) != m_sentPublishIDs.end())
				{
					return true;
				}

				--m_sendAllowance;
				m_sentPublishIDs.insert(packetId);
				return true;
			}

			bool hasSendAllowance() const noexcept
			{
				return m_sendAllowance > 0;
			}

			std::uint32_t getCurrentReceiveAllowance() const noexcept
			{
				return m_receiveAllowance;
			}

			std::uint32_t getCurrentSendAllowance() const noexcept
			{
				return m_sendAllowance;
			}

			std::uint32_t getMaxReceiveAllowance() const noexcept
			{
				return m_maxReceiveAllowance;
			}

			std::uint32_t getMaxSendAllowance() const noexcept
			{
				return m_maxSendAllowance;
			}

		private:
			std::uint32_t m_receiveAllowance{ RECEIVE_MAXIMUM_DEFAULT }; //Current allowance of PUBLISH packets that can be received. 
			std::uint32_t m_sendAllowance{ RECEIVE_MAXIMUM_DEFAULT }; //Current allowance of PUBLISH packets that can be sent.
			std::uint32_t m_maxReceiveAllowance{ RECEIVE_MAXIMUM_DEFAULT }; //Maximum allowance of PUBLISH packets that can be received.
			std::uint32_t m_maxSendAllowance{ RECEIVE_MAXIMUM_DEFAULT }; //Maximum allowance of PUBLISH packets that can be sent.

			std::unordered_set<std::uint16_t> m_sentPublishIDs; //Track sent publish packet IDs to avoid double decrementing send allowance.
			std::unordered_set<std::uint16_t> m_receivedPublishIDs; //Track received publish packet IDs to avoid double decrementing receive allowance.
		};
	}
}

#endif //INCLUDE_PRIVATE_KMMQTT_MQTT_RECEIVEMAXIMUMTRACKER_H