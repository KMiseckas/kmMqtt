// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H

#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <vector>

namespace kmMqtt
{
	namespace mqtt
	{
		struct UnSubscribePayloadHeader : IEncodeHeader
		{
			UnSubscribePayloadHeader() noexcept = default;
			UnSubscribePayloadHeader(std::vector<UTF8String>&& topics) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::vector<UTF8String> topics;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_HEADERS_UNSUBSCRIBEPAYLOADHEADER_H
