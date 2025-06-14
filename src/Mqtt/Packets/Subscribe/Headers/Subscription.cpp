#include "cleanMqtt/Mqtt/Packets/Subscribe/Headers/Subscription.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			Subscription::Subscription(const char* topic, EncodedSubscribeOptionsFlags opts) noexcept
				: topicFilter{ topic }, options{ std::move(opts) }
			{
				// Force reserved bits to zero (as per MQTT 5 spec)
				options.setFlagValue(SubscribeOptionsFlags::RESERVED, 0);
			}

			std::size_t Subscription::getEncodedBytesSize() const noexcept
			{
				return topicFilter.encodingSize() + sizeof(options);
			}

			void Subscription::encode(ByteBuffer& buffer) const
			{
				topicFilter.encode(buffer);
				buffer += options.getFlags();
			}
		}
	}
}
