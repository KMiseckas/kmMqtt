#ifndef INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_H
#define INCLUDE_CLEANMQTT_MQTT_SESSIONSTATE_H

#include <cleanMqtt/GlobalTypes.h>
#include "cleanMqtt/Utils/PacketIdPool.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include "cleanMqtt/Mqtt/SessionState/ISessionStatePersistantStore.h"
#include "cleanMqtt/Mqtt/SessionState/MessageContainer.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"

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
            * @param persistantStore Optional persistent store for session state.
            */
            SessionState(const char* clientId,
                std::uint32_t sessionExpiryInterval,
                std::uint32_t retryInterval = 0U,
                ISessionStatePersistantStore* persistantStore = nullptr);

            /**
            * @brief Adds a message to the session state.
            * 
            * @param packetId The packet identifier.
            * @param publishMsgData Data for publish packet.
            */
            ClientErrorCode addMessage(const PacketID packetId, PublishMessageData publishMsgData) noexcept;

            /**
			 * @brief Updates the status of a message in the session state and any other relevant data.
			 * 
			 * @param packetId The packet identifier of the message to update.
			 * @param newStatus The new status to set for the message.
             */
			void updateMessage(const PacketID packetId, PublishMessageStatus newStatus) noexcept;

            /**
            * @brief Removes a message from the session state by packet ID.
            * 
            * @param packetId The packet identifier to remove.
            */
            void removeMessage(const PacketID packetId) noexcept;

            /**
            * @brief Gets the message container for the session state (const).
            * 
            * @return Const reference to the MessageContainer.
            */
            const MessageContainer& messages() noexcept { return m_messages; }

            /**
            * @brief Clears all messages from the session state.
            */
            void clear() noexcept;

        protected:

			bool shouldBringToFront(PublishMessageStatus oldStatus, PublishMessageStatus newStatus) const noexcept;

		private:
			const char* m_clientId;
            Milliseconds m_sessionExpiryInterval;
            Milliseconds m_retryInterval;
			ISessionStatePersistantStore* m_persistantStore{ nullptr };
            MessageContainer m_messages{};
		};
	}
}

#endif
