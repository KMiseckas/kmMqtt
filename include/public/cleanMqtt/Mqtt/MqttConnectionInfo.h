#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/GlobalTypes.h"
#include "cleanMqtt/Mqtt/Enums/MqttVersion.h"
#include "cleanMqtt/Mqtt/Params/ConnectArgs.h"
#include "cleanMqtt/Mqtt/Params/ConnectAddress.h"
#include "cleanMqtt/Mqtt/State/SessionState/SessionState.h"
#include "cleanMqtt/Mqtt/State/PendingSubscription.h"
#include "cleanMqtt/Mqtt/State/PendingUnSubscription.h"
#include <cleanMqtt/Mqtt/TopicAliases.h>

#include <string>
#include <cstdint>

namespace cleanMqtt 
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

#endif //INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H 
