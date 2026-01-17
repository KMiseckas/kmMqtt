#ifndef INCLUDE_KMMQTT_MQTT_MQTTVERSION_H
#define INCLUDE_KMMQTT_MQTT_MQTTVERSION_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class MqttVersion : std::uint8_t
		{
			MQTT_3_1_1 = 4U,
			MQTT_5_0 = 5U,
		};
	}

}

#endif //INCLUDE_KMMQTT_MQTT_MQTTVERSION_H 