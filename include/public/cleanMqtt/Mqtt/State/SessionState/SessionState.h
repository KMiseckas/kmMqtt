#ifndef INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_H
#define INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_H

#include <cleanMqtt/GlobalTypes.h>
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include "cleanMqtt/Mqtt/State/SessionState/ISessionStatePersistantStore.h"
#include "cleanMqtt/Mqtt/State/SessionState/MessageContainer.h"
#include "cleanMqtt/Mqtt/Enums/ClientErrorCode.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		struct SessionState
		{
            /**
            * @brief Constructs a new SessionState object.
            *
            * @param clientId The client identifier.
            * @param sessionExpiryInterval The session expiry interval in seconds.
            * @param retryInterval The retry interval for message delivery attempts.
            */
            SessionState(const char* clientId,
                std::uint32_t sessionExpiryInterval,
                std::uint32_t retryInterval = 0U) noexcept;

            /**
			 * @brief Copy constructor.
			 * 
			 * @param other The SessionState object to copy from.
             */
			SessionState(const SessionState&) noexcept;

			SessionState& operator=(const SessionState&) noexcept;

            /**
			 * @brief Adds previous session state messages to the current session state.
			 * @param prevSessionState The previous session state to add messages from.
			 * @return ClientErrorCode indicating success or failure.
             */
            ClientErrorCode addPrevSessionState(const SessionState& prevSessionState) noexcept;

            /**
            * @brief Adds a message to the session state.
            * 
            * @param packetId The packet identifier.
            * @param publishMsgData Data for publish packet.
            */
            ClientErrorCode addMessage(const std::uint16_t packetId, PublishMessageData publishMsgData) noexcept;

            /**
			 * @brief Updates the status of a message in the session state and any other relevant data.
			 * 
			 * @param packetId The packet identifier of the message to update.
			 * @param newStatus The new status to set for the message.
             */
			void updateMessage(const std::uint16_t packetId, PublishMessageStatus newStatus) noexcept;

            /**
            * @brief Removes a message from the session state by packet ID.
            * 
            * @param packetId The packet identifier to remove.
            */
            void removeMessage(const std::uint16_t packetId) noexcept;

            /**
            * @brief Gets the message container for the session state (const).
            * 
            * @return Const reference to the MessageContainer.
            */
            const MessageContainer& messages() const noexcept { return m_messages; }

            /**
            * @brief Clears all messages from the session state.
            */
            void clear() noexcept;

        protected:

			bool shouldBringToFront(PublishMessageStatus oldStatus, PublishMessageStatus newStatus) const noexcept;

		private:

            ClientErrorCode addPrevStateMessage(const std::uint16_t packetId, const PublishMessageData& publishMsgData) noexcept;

			const char* m_clientId;
            Milliseconds m_retryInterval;
            std::shared_ptr<ISessionStatePersistantStore> m_persistantStore{ nullptr };
            Milliseconds m_sessionExpiryInterval;
            MessageContainer m_messages{};
			mutable std::mutex m_mutex{};
		};
	}
}

#endif
