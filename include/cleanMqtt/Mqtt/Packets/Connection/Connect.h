#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECT_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECT_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/ConnectPayloadHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			namespace
			{
				constexpr std::uint8_t k_ConnectFixedHeaderFlags = 0U;
			}

			class Connect : public BasePacket
			{
			public:
				Connect(ConnectVariableHeader&& variableHeader, ConnectPayloadHeader&& payloadHeader) noexcept;
				Connect(ByteBuffer&& dataBuffer) noexcept;
				~Connect() override;

				PacketType getPacketType() const noexcept override;

				const ConnectVariableHeader& getVariableHeader() const;
				const ConnectPayloadHeader& getPayloadHeader() const;

				void setUpHeaders() noexcept;

			private:
				ConnectVariableHeader m_variableHeader;
				ConnectPayloadHeader m_payloadHeader;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_CONNECT_H