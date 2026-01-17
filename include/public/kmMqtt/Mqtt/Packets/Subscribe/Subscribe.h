// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribePayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		namespace
		{
			constexpr std::uint8_t k_SubscribeFixedHeaderFlags = 0x02U;
		}

		class Subscribe : public BasePacket
		{
		public:
			Subscribe(SubscribeVariableHeader&& variableHeader, SubscribePayloadHeader&& payloadHeader) noexcept;
			Subscribe(ByteBuffer&& dataBuffer) noexcept;
			Subscribe(Subscribe&& other) noexcept;
			~Subscribe() override;

			PacketType getPacketType() const noexcept override;

			const SubscribeVariableHeader& getVariableHeader() const;
			const SubscribePayloadHeader& getPayloadHeader() const;

		private:
			void setUpHeaders() noexcept;

			SubscribeVariableHeader* m_variableHeader{ nullptr };
			SubscribePayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_SUBSCRIBE_H
