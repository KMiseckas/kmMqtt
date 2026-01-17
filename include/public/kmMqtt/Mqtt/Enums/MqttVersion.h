// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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