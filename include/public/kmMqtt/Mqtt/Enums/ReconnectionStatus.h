// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H
#define INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class ReconnectionStatus : std::uint8_t
		{
			RECONNECTING,
			SUCCEEDED,
			FAILED,
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H 
