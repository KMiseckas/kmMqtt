#ifndef INCLUDE_KMMQTT_MQTT_MQTTCONNECTIONINFO_H
#define INCLUDE_KMMQTT_MQTT_MQTTCONNECTIONINFO_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/GlobalTypes.h"
#include "kmMqtt/Mqtt/Enums/MqttVersion.h"
#include "kmMqtt/Mqtt/Params/ConnectArgs.h"
#include "kmMqtt/Mqtt/Params/ConnectAddress.h"
#include "kmMqtt/Mqtt/State/SessionState/SessionState.h"
#include "kmMqtt/Mqtt/State/PendingSubscription.h"
#include "kmMqtt/Mqtt/State/PendingUnSubscription.h"
#include <kmMqtt/Mqtt/TopicAliases.h>

#include <string>
#include <cstdint>

namespace kmMqtt 
{
	namespace mqtt
	{
		struct SessionState;

		struct PUBLIC_API MqttConnectionInfo
		{
			ConnectArgs connectArgs{ "CLIENT ID" };
			ConnectAddress connectAddress;
			ReconnectAddress reconnectAddress;

			bool hasBeenConnected{ false };
			bool awaitingPingResponse{ false };
			
			TimePoint connectionStartTime;
			TimePoint lastControlPacketTime;
			TimePoint lastPingReqSentTime;

			Milliseconds pingInterval{ 0U };
			std::uint16_t maxServerTopicAlias{ 0U };
			std::size_t maxServerPacketSize{ MAX_PACKET_SIZE };
			std::uint16_t serverKeepAlive{ 0U };
			bool isRetainAvailable{ true };
			bool subscribeIdentifiersSupported{ true };
			std::uint32_t receiveMaximumAsClient{ RECEIVE_MAXIMUM_DEFAULT };
			std::uint32_t receiveMaximumAsServer{ RECEIVE_MAXIMUM_DEFAULT };
			SessionState sessionState{ "", 0 };
			std::vector<PendingSubscription> pendingSubscriptions;
			std::vector<PendingUnSubscription> pendingUnSubscriptions;
			TopicAliases topicAliases;

			void clear(bool clearSessionState = false) noexcept;

		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_MQTTCONNECTIONINFO_H 
