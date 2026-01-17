// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H

#include <kmMqtt/Interfaces/IDecodeHeader.h>
#include <kmMqtt/Mqtt/Packets/Properties.h>
#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		struct SubscribeAckVariableHeader : IDecodeHeader
		{
			SubscribeAckVariableHeader() noexcept = default;

			DecodeResult decode(const ByteBuffer& buffer) noexcept override;

			std::uint16_t packetId{ 0U };
			Properties properties;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEACKVARIABLEHEADER_H
