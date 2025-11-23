#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectAckVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ConnectAckVariableHeader::ConnectAckVariableHeader() noexcept
		{
			flags.setFlagValue(ConnectAcknowledgeFlags::RESERVED, 0);
		}

		DecodeResult ConnectAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			try
			{
				flags.overrideFlags(buffer.readUint8());
				reasonCode = static_cast<ConnectReasonCode>(buffer.readUint8());
			}
			catch (const std::out_of_range& e)
			{
				return DecodeResult(DecodeErrorCode::MALFORMED_PACKET, e.what());
			}

			result = properties.decode(buffer);

			return result;
		}
	}
}