#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			Disconnect::Disconnect(DisconnectVariableHeader&& varHeader) noexcept
				: BasePacket(FixedHeaderFlags(k_DisconnectFixedHeaderFlags)),
				variableHeader(std::move(varHeader))
			{
				setUpHeaders();
			}

			Disconnect::Disconnect(ByteBuffer&& dataBuffer) noexcept
				: BasePacket(std::move(dataBuffer))
			{
				setUpHeaders();
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
				return variableHeader;
			}

			void Disconnect::setUpHeaders() noexcept
			{
				addDecodeHeader(&variableHeader);
				addEncodeHeader(&variableHeader);
			}
		}
	}
}