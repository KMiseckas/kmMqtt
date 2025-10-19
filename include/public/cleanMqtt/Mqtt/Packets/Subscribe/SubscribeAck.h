#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckPayloadHeader.h>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBEACK_H
