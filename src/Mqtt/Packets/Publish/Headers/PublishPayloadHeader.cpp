#include <cleanMqtt/Mqtt/Packets/Publish/Headers/PublishPayloadHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		PublishPayloadHeader::PublishPayloadHeader() noexcept
		{
		}

		PublishPayloadHeader::PublishPayloadHeader(BinaryData&& payload) noexcept
			: payload{ std::move(payload) }
		{
		}

		DecodeResult PublishPayloadHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			payload.decode(buffer);

			return result;
		}

		void PublishPayloadHeader::encode(ByteBuffer& buffer) const
		{
			payload.encode(buffer);
		}

		std::size_t PublishPayloadHeader::getEncodedBytesSize() const noexcept
		{
			return payload.encodingSize();
		}
	}
}