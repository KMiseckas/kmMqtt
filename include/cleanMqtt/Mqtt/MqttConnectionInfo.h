#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/MqttVersion.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>

#include <string>
#include <cstdint>

namespace cleanMqtt 
{
	namespace mqtt
	{
		struct PUBLIC_API MqttConnectionInfo
		{
			ConnectArgs connectArgs;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_MQTTCONNECTIONINFO_H 
