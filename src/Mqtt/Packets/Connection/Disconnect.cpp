#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			Disconnect::Disconnect(DisconnectVariableHeader&& varHeader) noexcept
				: BasePacket(FixedHeaderFlags(k_DisconnectFixedHeaderFlags)),
				m_variableHeader(std::move(varHeader))
			{
				setUpHeaders();
			}

			Disconnect::Disconnect(ByteBuffer&& dataBuffer) noexcept
				: BasePacket(std::move(dataBuffer))
			{
				setUpHeaders();
			}

			Disconnect::Disconnect(Disconnect&& other) noexcept
				: BasePacket{ std::move(other) },
				m_variableHeader{ std::move(other.m_variableHeader) }
			{
			}

			Disconnect::~Disconnect()
			{
			}

			PacketType Disconnect::getPacketType() const noexcept
			{
				return PacketType::DISCONNECT;
			}

			const DisconnectVariableHeader& Disconnect::getVariableHeader() const
			{
				return m_variableHeader;
			}

			void Disconnect::setUpHeaders() noexcept
			{
				addDecodeHeader(&m_variableHeader);
				addEncodeHeader(&m_variableHeader);
			}
		}
	}
}