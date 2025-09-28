#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H

#include <cleanMqtt/Utils/Event.h>
#include <cleanMqtt/Mqtt/ClientError.h>
#include <cleanMqtt/Mqtt/Enums/ReconnectionStatus.h>
#include "cleanMqtt/Mqtt/State/UnSubAckTopicReason.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/DataTypes.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h"
#include <cleanMqtt/Mqtt/Transport/SendResultData.h>
#include <cleanMqtt/Mqtt/State/SubAckTopicReason.h>
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

			bool isSuccessful{ false };
			bool hasReceivedAck{ false };
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
			packets::DisconnectReasonCode reasonCode;
			bool isBrokerInduced{ false };
			bool isGraceful{ false };
			ClientError error{};
		};

		struct PublishEventDetails
		{
			std::string topic{ "" };
			const packets::BinaryData* payload{ nullptr };
		};

		struct PublishAckEventDetails
		{
			std::uint16_t packetId{ 0 };
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
		using ConnectEvent = events::Event<const ConnectEventDetails&, const packets::ConnectAck&>;
		using ReconnectEvent = events::Event<const ReconnectEventDetails&, const packets::ConnectAck&>;
		using DisconnectEvent = events::Event<const DisconnectEventDetails&>;
		using PublishEvent = events::Event<const PublishEventDetails&, const packets::Publish&>;
		using PublishAckEvent = events::Event<const PublishAckEventDetails&, const packets::PublishAck&>;
		using SubscribeAckEvent = events::Event<const SubscribeAckEventDetails&, const packets::SubscribeAck&>;
		using UnSubscribeAckEvent = events::Event<const UnSubscribeAckEventDetails&, const packets::UnSubscribeAck&>;
	}
}

#endif // INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTEVENTS_H
