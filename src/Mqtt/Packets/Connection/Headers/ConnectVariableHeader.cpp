#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		ConnectVariableHeader::ConnectVariableHeader() noexcept
		{
			flags.setFlagValue(ConnectFlags::RESERVED, false);
		}

		ConnectVariableHeader::ConnectVariableHeader(
			const char* protocolName,
			MqttVersion mqttVersion,
			std::uint16_t keepAlive,
			EncodedConnectFlags&& flags,
			Properties&& properties) noexcept
			: protocolName(protocolName),
			protocolLevel(mqttVersion),
			keepAliveInSec(keepAlive),
			flags(std::move(flags)),
			properties(std::move(properties))
		{
			flags.setFlagValue(ConnectFlags::RESERVED, false);
		}

		void ConnectVariableHeader::encode(ByteBuffer& buffer) const
		{
			assert(static_cast<std::uint8_t>(protocolLevel) != 0);

			protocolName.encode(buffer);
			buffer += static_cast<std::uint8_t>(protocolLevel);
			buffer += flags.getFlags();
			buffer.append(keepAliveInSec);
			properties.encode(buffer);
		}

		std::size_t ConnectVariableHeader::getEncodedBytesSize() const noexcept
		{
			return protocolName.encodingSize() +
				sizeof(std::underlying_type<MqttVersion>::type) +
				sizeof(std::underlying_type<ConnectFlags>::type) +
				sizeof(keepAliveInSec) +
				properties.encodingSize();
		}
	}
}
