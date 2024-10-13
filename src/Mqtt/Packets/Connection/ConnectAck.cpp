#include <cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			ConnectAck::ConnectAck() noexcept
				: BasePacket(FixedHeaderFlags(k_ConnectAckFixedHeaderFlags))
			{
				setUpHeaders();
			}

			ConnectAck::ConnectAck(ByteBuffer&& dataBuffer) noexcept
				: BasePacket(std::move(dataBuffer))
			{
				setUpHeaders();
			}

			ConnectAck::~ConnectAck()
			{
			}

			PacketType ConnectAck::getPacketType() const noexcept
			{
				return PacketType::CONNECT_ACKNOWLEDGE;
			}

			void ConnectAck::setUpHeaders() noexcept
			{
				addDecodeHeader(&m_variableHeader);
			}
		}
	}
}