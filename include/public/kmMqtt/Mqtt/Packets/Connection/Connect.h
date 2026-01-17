// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECT_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECT_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Connection/Headers/ConnectVariableHeader.h>
#include <kmMqtt/Mqtt/Packets/Connection/Headers/ConnectPayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
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
			Connect(Connect&& other) noexcept;
			Connect() noexcept = default;
			~Connect() override;

			PacketType getPacketType() const noexcept override;

			const ConnectVariableHeader& getVariableHeader() const;
			const ConnectPayloadHeader& getPayloadHeader() const;

		private:
			void setUpHeaders() noexcept;

			ConnectVariableHeader* m_variableHeader{ nullptr };
			ConnectPayloadHeader* m_payloadHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CONNECT_H