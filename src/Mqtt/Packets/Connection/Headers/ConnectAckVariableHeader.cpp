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

			flags.overrideFlags(buffer.readUint8());
			reasonCode = static_cast<ConnectReasonCode>(buffer.readUint8());

			result = std::move(properties.decode(buffer));

			return result;
		}
	}
}