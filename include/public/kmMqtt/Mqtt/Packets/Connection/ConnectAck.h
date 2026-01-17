#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Connection/Headers/ConnectAckVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
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
			ConnectAck(ConnectAck&& other) noexcept;
			~ConnectAck() override;

			ConnectAck& operator=(ConnectAck&& other) noexcept;

			PacketType getPacketType() const noexcept override;

			const ConnectAckVariableHeader& getVariableHeader() const noexcept;

		protected:
			void setUpHeaders() noexcept;

			ConnectAckVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECTACK_H
