// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class ConnectAcknowledgeFlags : std::uint8_t
		{
			SESSION_PRESENT = 1 << 0,
			RESERVED = 0b11111110
		};

		ENUM_FLAG_OPERATORS(ConnectAcknowledgeFlags)

			using EncodedConnectAcknowledgeFlags = Flags<std::uint8_t, ConnectAcknowledgeFlags, 255>;
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H
