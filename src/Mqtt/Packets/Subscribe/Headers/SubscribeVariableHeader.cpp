#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			SubscribeVariableHeader::SubscribeVariableHeader() noexcept
				: packetId(0), properties()
			{
			}

			SubscribeVariableHeader::SubscribeVariableHeader(std::uint16_t packetId, Properties&& properties) noexcept
				: packetId(packetId), properties(std::move(properties))
			{
			}

			void SubscribeVariableHeader::encode(ByteBuffer& buffer) const
			{
				buffer.append(packetId);
				properties.encode(buffer);
			}

			std::size_t SubscribeVariableHeader::getEncodedBytesSize() const noexcept
			{
				return sizeof(packetId) + properties.encodingSize();
			}
		}
	}
}
