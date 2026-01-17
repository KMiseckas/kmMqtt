// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H
#define INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H

#include <cstdint>

namespace kmMqtt 
{
	namespace mqtt
	{
		enum class ConnectionStatus : std::uint8_t
		{
			DISCONNECTED,
			CONNECTED,
			CONNECTING,
			RECONNECT,
			RECONNECTING,
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H 
