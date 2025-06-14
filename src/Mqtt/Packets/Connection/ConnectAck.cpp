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

			ConnectAck::ConnectAck(ConnectAck&& other) noexcept
				: BasePacket(std::move(other)),
				m_variableHeader(std::move(other.m_variableHeader))
			{
				other.m_variableHeader = nullptr;

				setUpHeaders();
			}

			ConnectAck::~ConnectAck()
			{
				delete m_variableHeader;
			}

			PacketType ConnectAck::getPacketType() const noexcept
			{
				return PacketType::CONNECT_ACKNOWLEDGE;
			}

			const ConnectAckVariableHeader& ConnectAck::getVariableHeader() const noexcept
			{
				return *m_variableHeader;
			}

			void ConnectAck::setUpHeaders() noexcept
			{
				if (m_variableHeader == nullptr)
				{
					m_variableHeader = new ConnectAckVariableHeader();
				}

				addDecodeHeader(m_variableHeader);
			}
		}
	}
}