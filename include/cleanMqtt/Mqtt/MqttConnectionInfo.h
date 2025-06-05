#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/GlobalTypes.h"
#include "cleanMqtt/Mqtt/Enums/MqttVersion.h"
#include "cleanMqtt/Mqtt/Params/ConnectArgs.h"
#include "cleanMqtt/Mqtt/Params/ConnectAddress.h"
#include "cleanMqtt/Mqtt/SessionState/SessionState.h"

#include <string>
#include <cstdint>

namespace cleanMqtt 
{
	namespace mqtt
	{
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
			std::uint16_t serverKeepAlive{ 0U };
			bool isRetainAvailable{ true };
			SessionState sessionState{ "", 0 };
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H 
