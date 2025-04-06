#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/MqttVersion.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include <cleanMqtt/Mqtt/Params/ConnectAddress.h>

#include <string>
#include <cstdint>

namespace cleanMqtt 
{
	namespace mqtt
	{
		struct PUBLIC_API MqttConnectionInfo
		{
			using TimePoint = std::chrono::steady_clock::time_point;
			using Seconds = std::chrono::seconds;
			using Milliseconds = std::chrono::milliseconds;

			ConnectArgs connectArgs{ "CLIENT ID" };
			ConnectAddress connectAddress;
			ReconnectAddress reconnectAddress;

			bool hasBeenConnected{ false };
			bool awaitingPingResponse{ false };
			
			TimePoint connectionStartTime;
			TimePoint lastControlPacketTime;
			TimePoint lastPingReqSentTime;

			Seconds pingInterval{ 0U };
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H 
