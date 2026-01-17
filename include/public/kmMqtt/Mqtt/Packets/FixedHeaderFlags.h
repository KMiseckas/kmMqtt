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