#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		DecodeResult SubscribeAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			packetId = buffer.readUInt16();

			if (packetId == 0)
			{
				result.code = DecodeErrorCode::PROTOCOL_ERROR;
				result.reason = "Packet ID cannot be zero in SUBACK";
				return result;
			}

			if (buffer.readHeadroom() > 0)
			{
				result = properties.decode(buffer);
			}

			return result;
		}
	}
}