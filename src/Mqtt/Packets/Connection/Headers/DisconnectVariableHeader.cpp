#include <cleanMqtt/Mqtt/Packets/Connection/Headers/DisconnectVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			DisconnectVariableHeader::DisconnectVariableHeader() noexcept
			{
			}

			DisconnectVariableHeader::DisconnectVariableHeader(DisconnectReasonCode disconnectReasonCode, Properties&& disconnectProperties) noexcept
				: reasonCode(disconnectReasonCode), properties(std::move(disconnectProperties))
			{
			}

			void DisconnectVariableHeader::encode(ByteBuffer& buffer) const
			{
				//Spec allows to skip encoding of data in following circumstances.
				if (reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION && properties.size() == 0)
				{
					return;
				}

				buffer += static_cast<std::uint8_t>(reasonCode);
				properties.encode(buffer);
			}

			std::size_t DisconnectVariableHeader::getEncodedBytesSize() const noexcept
			{
				//Spec allows to skip encoding of data in following circumstances.
				if (reasonCode == DisconnectReasonCode::NORMAL_DISCONNECTION && properties.size() == 0)
				{
					return 0;
				}

				return sizeof(reasonCode) +
					properties.encodingSize();
			}

			void DisconnectVariableHeader::decode(const ByteBuffer& buffer) noexcept
			{
				reasonCode = static_cast<DisconnectReasonCode>(buffer.readUint8());
				properties.decode(buffer);
			}
		}
	}
}
