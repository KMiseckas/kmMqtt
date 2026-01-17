// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H


#include "kmMqtt/Mqtt/Packets/Subscribe/Flags/SubscribeOptionsFlags.h"
#include "kmMqtt/Mqtt/Packets/DataTypes.h"
#include <cstdint>
#include <vector>

namespace kmMqtt
{
	namespace mqtt
	{
		struct Subscription
		{
			Subscription() noexcept = default;
			Subscription(const char* topic, EncodedSubscribeOptionsFlags opts) noexcept;

			std::size_t getEncodedBytesSize() const noexcept;

			void encode(ByteBuffer& buffer) const;

			UTF8String topicFilter;
			EncodedSubscribeOptionsFlags options;
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_SUBSCRIBE_HEADERS_SUBSCRIBE_HEADERS_SUBSCRIPTION_H
