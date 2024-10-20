#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Headers/DisconnectVariableHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			namespace
			{
				constexpr std::uint8_t k_DisconnectFixedHeaderFlags = 0U;
			}

			class Disconnect : public BasePacket
			{
			public:
				Disconnect(DisconnectVariableHeader&& varHeader) noexcept;
				Disconnect(ByteBuffer&& dataBuffer) noexcept;
				Disconnect(Disconnect&& other) noexcept;
				~Disconnect() override;

				PacketType getPacketType() const noexcept override;

				const DisconnectVariableHeader& getVariableHeader() const;

				void setUpHeaders() noexcept;

			private:
				DisconnectVariableHeader m_variableHeader;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H