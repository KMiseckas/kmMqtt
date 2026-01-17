// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H

#include <kmMqtt/Mqtt/Packets/BasePacket.h>
#include <kmMqtt/Mqtt/Packets/Connection/Headers/DisconnectVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
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
			DisconnectVariableHeader* m_variableHeader{ nullptr };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_DISCONNECT_H