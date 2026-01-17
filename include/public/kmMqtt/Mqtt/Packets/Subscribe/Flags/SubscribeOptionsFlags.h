// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class SubscribeOptionsFlags : std::uint8_t
		{
			QOS = 1 << 0 | 1 << 1,
			NL = 1 << 2,
			RAP = 1 << 3,
			RETAIN_HANDLING = 1 << 4 | 1 << 5,
			RESERVED = 1 << 6 | 1 << 7
		};

		ENUM_FLAG_OPERATORS(SubscribeOptionsFlags)

			using EncodedSubscribeOptionsFlags = Flags<std::uint8_t, SubscribeOptionsFlags, 255>;
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H 