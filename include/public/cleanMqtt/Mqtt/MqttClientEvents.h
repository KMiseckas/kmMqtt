#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H

#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/DataTypes.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h"
#include "cleanMqtt/Mqtt/State/SubAckTopicReason.h"
#include "cleanMqtt/Mqtt/State/UnSubAckTopicReason.h"
#include "Packets/Publish/Publish.h"
#include "Packets/Publish/PublishComp.h"
#include "Packets/Publish/PublishRec.h"
#include "Packets/Publish/PublishRel.h"
#include <cleanMqtt/Mqtt/ClientError.h>
#include "cleanMqtt/Mqtt/Enums/ReconnectionStatus.h"
#include "cleanMqtt/Mqtt/Transport/SendResultData.h"
#include "cleanMqtt/Utils/Event.h"

#include <cstdint>
#include <cstring>

namespace cleanMqtt
{
	namespace interfaces
	{
		struct SendResultData;
	}

	namespace mqtt
	{
		//Event data structures
		struct ConnectEventDetails
		{
			ConnectEventDetails(bool isSuccessful, bool hasReceivedAck, ClientError error = ClientErrorCode::No_Error)
				: isSuccessful(isSuccessful), hasReceivedAck(hasReceivedAck), error(error) {
			}

			//Whether the connection attempt was successful. Can fail internally without receiving a CONNACK packet.
			bool isSuccessful{ false };

			//Whether a CONNACK packet was received from the broker regardless of failure or success.
			bool hasReceivedAck{ false };

			//Error code if any error occurred during internally during connection attempt.
			ClientError error{};
		};

		struct ReconnectEventDetails
		{
			ReconnectionStatus status;
			bool hasReceivedAck{ false };
			int attemptCount{ 0 };
			ClientError error{};
		};

		struct DisconnectEventDetails
		{
			DisconnectReasonCode reasonCode;
			bool isBrokerInduced{ false };
			bool isGraceful{ false };
			ClientError error{};
		};

		struct PublishEventDetails
		{
			std::string topic{ "" };
			const ByteBuffer* payload{ nullptr };
		};

		struct PublishCompleteEventDetails
		{
			PacketType packetType{ PacketType::RESERVED };
			std::uint16_t packetId{ 0 };
			std::uint8_t reasonCode{ 0 };

			/**
			 * @brief Checks if the publish operation was successful based on the reason code.
			 * @return true if the reason code is success; false otherwise.
			 */
			bool isSuccess() const noexcept
			{
				static_assert(static_cast<std::uint8_t>(PubAckReasonCode::SUCCESS) == 0, "PubAckReasonCode::SUCCESS must be 0 for isSuccess() to work correctly.");
				static_assert(static_cast<std::uint8_t>(PubCompReasonCode::SUCCESS) == 0, "PubCompReasonCode::SUCCESS must be 0 for isSuccess() to work correctly.");
				static_assert(static_cast<std::uint8_t>(PubRecReasonCode::SUCCESS) == 0, "PubRecReasonCode::SUCCESS must be 0 for isSuccess() to work correctly.");
				static_assert(static_cast<std::uint8_t>(PubRelReasonCode::SUCCESS) == 0, "PubRelReasonCode::SUCCESS must be 0 for isSuccess() to work correctly.");

				return reasonCode == 0;
			}
		};

		struct SubscribeAckEventDetails
		{
			std::uint16_t packetId{ 0 };
			SubAckResults results{};
		};

		struct UnSubscribeAckEventDetails
		{
			std::uint16_t packetId{ 0 };
			UnSubAckResults results{};
		};

		//Public events
		using ErrorEvent = events::Event<ClientError, SendResultData>;
		using ConnectEvent = events::Event<const ConnectEventDetails&, const ConnectAck&>;
		using ReconnectEvent = events::Event<const ReconnectEventDetails&, const ConnectAck&>;
		using DisconnectEvent = events::Event<const DisconnectEventDetails&>;
		using PublishEvent = events::Event<const PublishEventDetails&, const Publish&>;
		using PublishCompletedEvent = events::Event<const PublishCompleteEventDetails&>;
		using SubscribeAckEvent = events::Event<const SubscribeAckEventDetails&, const SubscribeAck&>;
		using UnSubscribeAckEvent = events::Event<const UnSubscribeAckEventDetails&, const UnSubscribeAck&>;
	}
}

#endif // INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H
