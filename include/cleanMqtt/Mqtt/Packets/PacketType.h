#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKETTYPE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PACKETTYPE_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			enum class PacketType : std::uint8_t
			{
				RESERVED = 0U,
				CONNECT = 1U,
				CONNECT_ACKNOWLEDGE = 2U,
				PUBLISH = 3U,
				PUBLISH_ACKNOWLEDGE = 4U,
				PUBLISH_RECEIVED = 5U,
				PUBLISH_RELEASED = 6U,
				PUBLISH_COMPLETE = 7U,
				SUBSCRIBE = 8U,
				SUBSCRIBE_ACKNOWLEDGE = 9U,
				UNSUBSCRIBE = 10U,
				UNSUBSCRIBE_ACKNOWLEDGE = 11U,
				PING_REQUQEST = 12U,
				PING_RESPONSE = 13U,
				DISCONNECT = 14U,
				AUTH = 15U,
				_COUNT,
			};

			constexpr const char* k_packetTypeName[static_cast<std::uint8_t>(PacketType::_COUNT)]
			{
				"RESERVED",
				"CONNECT",
				"CONNECT_ACKNOWLEDGE",
				"PUBLISH",
				"PUBLISH_ACKNOWLEDGE",
				"PUBLISH_RECEIVED",
				"PUBLISH_RELEASED",
				"PUBLISH_COMPLETE",
				"SUBSCRIBE",
				"SUBSCRIBE_ACKNOWLEDGE",
				"UNSUBSCRIBE",
				"UNSUBSCRIBE_ACKNOWLEDGE",
				"PING_REQUQEST",
				"PING_RESPONSE",
				"DISCONNECT",
				"AUTH",
			};

			static inline const char* packetTypeToString(PacketType packetType)
			{
				if (packetType < PacketType::_COUNT)
				{
					return k_packetTypeName[static_cast<std::uint8_t>(packetType)];
				}
				return "UNKNOWN";
			}
		}
	}
}

#endif