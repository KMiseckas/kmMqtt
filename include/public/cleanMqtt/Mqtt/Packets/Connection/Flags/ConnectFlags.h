#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H

#include <cleanMqtt/Mqtt/Enums/Qos.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTFLAGS_H 