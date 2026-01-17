#include "kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeVariableHeader.h"

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribeVariableHeader::UnSubscribeVariableHeader() noexcept
			: packetId(0), properties()
		{
		}

		UnSubscribeVariableHeader::UnSubscribeVariableHeader(std::uint16_t packetId, Properties&& properties) noexcept
			: packetId(packetId), properties(std::move(properties))
		{
		}

		void UnSubscribeVariableHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(packetId);
			properties.encode(buffer);
		}

		std::size_t UnSubscribeVariableHeader::getEncodedBytesSize() const noexcept
		{
			return sizeof(packetId) + properties.encodingSize();
		}
	}
}