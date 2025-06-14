#include "cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribePayloadHeader.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			SubscribePayloadHeader::SubscribePayloadHeader(std::vector<Subscription>&& subscriptions) noexcept
				: subscriptions(std::move(subscriptions)) {}

			void SubscribePayloadHeader::encode(ByteBuffer& buffer) const
			{
				for (const auto& sub : subscriptions)
				{
					sub.encode(buffer);
				}
			}

			std::size_t SubscribePayloadHeader::getEncodedBytesSize() const noexcept
			{
				std::size_t size = 0;
				for (const auto& sub : subscriptions)
				{
					size += sub.getEncodedBytesSize();
				}
				return size;
			}
		}
	}
}
