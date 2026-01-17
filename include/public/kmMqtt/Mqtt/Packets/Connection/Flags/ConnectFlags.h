// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H

#include <kmMqtt/Mqtt/Enums/Qos.h>
#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class ConnectFlags : std::uint8_t
		{
			RESERVED = 1 << 0,
			CLEAN_START = 1 << 1,
			WILL_FLAG = 1 << 2,
			WILL_QOS = 1 << 3 | 1 << 4,
			WILL_RETAIN = 1 << 5,
			PASSWORD = 1 << 6,
			USER_NAME = 1 << 7,
		};

		ENUM_FLAG_OPERATORS(ConnectFlags)

			using EncodedConnectFlags = Flags<std::uint8_t, ConnectFlags, 255>;
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H 