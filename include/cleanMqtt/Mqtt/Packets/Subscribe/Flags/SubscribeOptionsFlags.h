#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
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
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEOPTIONSLAGS_H 