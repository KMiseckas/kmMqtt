// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct SubscribeVariableHeader : IEncodeHeader
		{
		public:
			SubscribeVariableHeader() noexcept;
			SubscribeVariableHeader(std::uint16_t packetId, Properties&& properties) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::uint16_t packetId{ 0 };
			Properties properties;
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEVARIABLEHEADER_H
