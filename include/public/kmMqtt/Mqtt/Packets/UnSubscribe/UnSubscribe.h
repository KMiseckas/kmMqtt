// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribePayloadHeader.h>
#include <vector>
#include <string>

namespace kmMqtt
{
	namespace mqtt
	{
		namespace
		{
			constexpr std::uint8_t k_UnSubscribeFixedHeaderFlags = 0x02U;
		}

		class UnSubscribe : public BasePacket
		{
		public:
			UnSubscribe(UnSubscribeVariableHeader&& variableHeader, UnSubscribePayloadHeader&& payloadHeader) noexcept;
			UnSubscribe(ByteBuffer&& dataBuffer) noexcept;
			UnSubscribe(UnSubscribe&& other) noexcept;
			~UnSubscribe() override;

			PacketType getPacketType() const noexcept override;
			const UnSubscribeVariableHeader& getVariableHeader() const;
			const UnSubscribePayloadHeader& getPayloadHeader() const;

		private:
			void setUpHeaders() noexcept;

			UnSubscribeVariableHeader* m_variableHeader{ nullptr };
			UnSubscribePayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_UNSUBSCRIBE_H
