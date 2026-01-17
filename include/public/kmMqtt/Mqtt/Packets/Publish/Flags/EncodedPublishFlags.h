#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H

#include <kmMqtt/Mqtt/Packets/FixedHeaderFlags.h>
#include <kmMqtt/Mqtt/Enums/Qos.h>
#include <kmMqtt/GlobalMacros.h>

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		namespace
		{
			enum class PublishFlags : std::uint8_t
			{
				IS_RETAINED = 1 << 0,
				QOS = 1 << 1 | 1 << 2,
				IS_DUPLICATE = 1 << 3,
			};

			ENUM_FLAG_OPERATORS(PublishFlags)
		}

		struct EncodedPublishFlags : public FixedHeaderFlags
		{
			EncodedPublishFlags(std::uint8_t flags) noexcept
				: FixedHeaderFlags(flags)
			{
			};

			EncodedPublishFlags(bool isDuplicate, const Qos& qos, bool isRetained) noexcept
				: FixedHeaderFlags(0U)
			{
				setFlagValue(static_cast<std::uint8_t>(PublishFlags::IS_RETAINED), isRetained);
				setFlagValue(static_cast<std::uint8_t>(PublishFlags::IS_DUPLICATE), isDuplicate);
				setFlagValue(static_cast<std::uint8_t>(PublishFlags::QOS), static_cast<std::uint8_t>(qos));
			};
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H 