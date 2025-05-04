#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H

#include <cleanMqtt/Mqtt/Packets/FixedHeaderFlags.h>
#include <cleanMqtt/Mqtt/Enums/Qos.h>
#include <cleanMqtt/GlobalMacros.h>

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			namespace
			{
				enum class PublishFlags : std::uint8_t
				{
					IS_RETAINED = 1 << 0,
					QOS = 1 << 1 | 1<< 2,
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
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_PUBLISH_FLAGS_PUBLISHFLAGS_H 