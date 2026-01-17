// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_QOS_H
#define INCLUDE_KMMQTT_MQTT_QOS_H

#include <cstdint>

namespace kmMqtt 
{
	namespace mqtt
	{
		enum class Qos : std::uint8_t
		{
			QOS_0 = 0U,
			QOS_1 = 1U,
			QOS_2 = 2U,
		};
	}

}

#endif //INCLUDE_KMMQTT_MQTT_QOS_H 