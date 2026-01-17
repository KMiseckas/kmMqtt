// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H

#include <kmMqtt/Interfaces/IEncodeHeader.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include "kmMqtt/Mqtt/Packets/Subscribe/Headers/Subscription.h"
#include <cstdint>
#include <vector>

namespace kmMqtt
{
	namespace mqtt
	{
		struct SubscribePayloadHeader : IEncodeHeader
		{
			SubscribePayloadHeader() noexcept = default;
			SubscribePayloadHeader(std::vector<Subscription>&& subscriptions) noexcept;

			void encode(ByteBuffer& buffer) const override;
			std::size_t getEncodedBytesSize() const noexcept override;

			std::vector<Subscription> subscriptions;
		};
	}
}

#endif // INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBEPAYLOADHEADER_H
