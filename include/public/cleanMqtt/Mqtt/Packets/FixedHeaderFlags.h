#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H

#include <cleanMqtt/Mqtt/Packets/Flags.h>

#include <cstdint>

namespace cleanMqtt
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

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_FIXEDHEADERFLAGS_H 