#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_FLAGS_CONNECTACKNOWLEDGEFLAGS_H
