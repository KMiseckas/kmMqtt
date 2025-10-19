#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H

#include <cleanMqtt/Mqtt/Packets/BasePacket.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckVariableHeader.h>
#include <cleanMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckPayloadHeader.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace
		{
			constexpr std::uint8_t k_UnSubscribeAckFixedHeaderFlags = 0x00U;
		}

		class UnSubscribeAck : public BasePacket
		{
		public:
			UnSubscribeAck(UnSubscribeAckVariableHeader&& variableHeader, UnSubscribeAckPayloadHeader&& payloadHeader) noexcept;
			UnSubscribeAck(ByteBuffer&& dataBuffer) noexcept;
			UnSubscribeAck(UnSubscribeAck&& other) noexcept;
			~UnSubscribeAck() override;

			PacketType getPacketType() const noexcept override;

			const UnSubscribeAckVariableHeader& getVariableHeader() const;
			const UnSubscribeAckPayloadHeader& getPayloadHeader() const;

		private:
			void setUpHeaders() noexcept;

			UnSubscribeAckVariableHeader* m_variableHeader{ nullptr };
			UnSubscribeAckPayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H
