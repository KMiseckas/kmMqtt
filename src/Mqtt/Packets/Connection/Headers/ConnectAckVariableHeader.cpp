#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectAckVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			ConnectAckVariableHeader::ConnectAckVariableHeader() noexcept
			{
				flags.setFlagValue(ConnectAcknowledgeFlags::RESERVED, 0);
			}

			void ConnectAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
			{
				flags.overrideFlags(buffer.readUint8());
				reasonCode = static_cast<ConnectReasonCode>(buffer.readUint8());
				properties.decode(buffer);
			}
		}
	}
}