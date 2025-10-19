#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectPayloadHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ConnectPayloadHeader::ConnectPayloadHeader() noexcept
		{
		}

		ConnectPayloadHeader::ConnectPayloadHeader(
			UTF8String&& clientId,
			Properties&& willProperties,
			UTF8String&& willTopic,
			BinaryData&& willPayload,
			UTF8String&& userName,
			BinaryData&& password) noexcept
			: clientId(std::move(clientId)),
			willProperties(std::move(willProperties)),
			willTopic(std::move(willTopic)),
			willPayload(std::move(willPayload)),
			userName(std::move(userName)),
			password(std::move(password))
		{
		}

		void ConnectPayloadHeader::encode(ByteBuffer& buffer) const
		{
			clientId.encode(buffer);

			if (willProperties.size() > 0) willProperties.encode(buffer);
			if (willTopic.stringSize() > 0) willTopic.encode(buffer);
			if (willPayload.size() > 0) willPayload.encode(buffer);
			if (password.size() > 0) password.encode(buffer);
			if (userName.stringSize() > 0) userName.encode(buffer);
		}

		std::size_t ConnectPayloadHeader::getEncodedBytesSize() const noexcept
		{
			auto size = clientId.encodingSize();

			if (willProperties.size() > 0) size += willProperties.encodingSize();
			if (willTopic.stringSize() > 0) size += willTopic.encodingSize();
			if (willPayload.size() > 0) size += willPayload.encodingSize();
			if (password.size() > 0) size += password.encodingSize();
			if (userName.stringSize() > 0) size += userName.encodingSize();

			return size;
		}
	}
}
