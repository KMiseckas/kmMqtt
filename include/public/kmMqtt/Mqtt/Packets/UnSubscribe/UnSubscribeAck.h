// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckPayloadHeader.h>

namespace kmMqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBEACK_H
