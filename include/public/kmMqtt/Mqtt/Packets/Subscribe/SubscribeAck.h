#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckPayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		namespace
		{
			constexpr std::uint8_t k_SubscribeAckFixedHeaderFlags = 0x00U;
		}

		class SubscribeAck : public BasePacket
		{
		public:
			SubscribeAck(SubscribeAckVariableHeader&& variableHeader, SubscribeAckPayloadHeader&& payloadHeader) noexcept;
			SubscribeAck(ByteBuffer&& dataBuffer) noexcept;
			SubscribeAck(SubscribeAck&& other) noexcept;
			~SubscribeAck() override;

			PacketType getPacketType() const noexcept override;

			const SubscribeAckVariableHeader& getVariableHeader() const;
			const SubscribeAckPayloadHeader& getPayloadHeader() const;

		private:
			void setUpHeaders() noexcept;

			SubscribeAckVariableHeader* m_variableHeader{ nullptr };
			SubscribeAckPayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H
