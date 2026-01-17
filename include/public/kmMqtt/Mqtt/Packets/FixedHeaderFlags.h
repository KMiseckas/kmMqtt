// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H

#include <kmMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct FixedHeaderFlags : public Flags<std::uint8_t, std::uint8_t, 15>
		{
			FixedHeaderFlags() = delete;
			FixedHeaderFlags(std::uint8_t flags = 0U) noexcept
				: Flags(flags)
			{
			};
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H 