#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectAckVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			namespace
			{
				constexpr std::uint8_t k_ConnectAckFixedHeaderFlags = 0U;
			}
			
			class ConnectAck : public BasePacket
			{
			public:
				ConnectAck() noexcept;
				ConnectAck(ByteBuffer&& dataBuffer) noexcept;
				~ConnectAck() override;

				PacketType getPacketType() const noexcept override;

				const ConnectAckVariableHeader& getVariableHeader() const noexcept;

			protected:
				void setUpHeaders() noexcept;

				ConnectAckVariableHeader m_variableHeader;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H
