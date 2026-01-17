// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/PacketUtils.h>
#include <kmMqtt/Logger/Log.h>
#include <stdexcept>

namespace kmMqtt
{
	namespace mqtt
	{
		PacketType checkPacketType(const std::uint8_t* data, std::size_t size)
		{
			if (size == 0)
			{
				LogException("Packet Type Check", std::invalid_argument("Data buffer size is 0. Cannot determine packet type from provided data buffer."));
			}

			return static_cast<PacketType>(data[0] >> 4);
		}
	}
}