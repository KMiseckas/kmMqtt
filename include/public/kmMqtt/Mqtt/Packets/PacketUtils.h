// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PACKETUTILS_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PACKETUTILS_H

#include <kmMqtt/Mqtt/Packets/PacketType.h>
#include <vector>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Check the type of packet the byte data represents. Use to check packet type for MQTT.
		 * @param data The data that contains the packet type, represented as bits in the first 4 bits of the data (MSB).
		 * @param bufferSize The size of the packet data.
		 * @return The type of packet.
		 */
		PacketType checkPacketType(const std::uint8_t* data, std::size_t size);

		/**
		 * @brief Check if the bytes passed in contain positive bits of contiguous nature.
		 * 0b0011 is contiguous, 0b0101 is not contiguous.
		 * @tparam TSizeType The type that represents the bytes to scan.
		 * @param bitmask The bytes to scan.
		 * @return True if positive bits anywhere along the bytes are contiguous until the next negative bit is met.
		 */
		template<typename TSizeType>
		constexpr bool arePositiveBitsContiguous(TSizeType bitmask)
		{
			if (bitmask == 0) return false;

			while ((bitmask & 1) == 0)
			{
				bitmask >>= 1;
			}

			return (bitmask & (bitmask + 1)) == 0;
		};

		/**
		 * @brief Get the position in the bytes of the first `1` bit encountered, LSB to MSB. 0 indexed.
		 * @tparam TSizeType The type representing bytes.
		 * @param bitmask The bytes to scan.
		 * @return The position of bit within the byte, with 0 being the first position at LSB.
		 */
		template<typename TSizeType>
		constexpr std::int8_t getFirstBitPosition(TSizeType bitmask)
		{
			if (bitmask == 0) return -1;

			std::uint8_t shiftAmount = 0;

			while ((bitmask & 1) == 0)
			{
				bitmask >>= 1;
				++shiftAmount;
			}

			return shiftAmount;
		};
	}
}

#endif