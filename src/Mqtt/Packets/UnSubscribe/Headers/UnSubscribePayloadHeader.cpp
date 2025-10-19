#include "cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribePayloadHeader.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		UnSubscribePayloadHeader::UnSubscribePayloadHeader(std::vector<UTF8String>&& topics) noexcept
			: topics(std::move(topics)) {
		}

		void UnSubscribePayloadHeader::encode(ByteBuffer& buffer) const
		{
			for (const auto& topic : topics)
			{
				topic.encode(buffer);
			}
		}

		std::size_t UnSubscribePayloadHeader::getEncodedBytesSize() const noexcept
		{
			std::size_t size = 0;
			for (const auto& topic : topics)
			{
				size += topic.encodingSize();
			}
			return size;
		}
	}
}
