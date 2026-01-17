// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Mqtt/Packets/Subscribe/Headers/Subscription.h"

namespace kmMqtt
{
	namespace mqtt
	{
		Subscription::Subscription(const char* topic, EncodedSubscribeOptionsFlags opts) noexcept
			: topicFilter{ topic }, options{ std::move(opts) }
		{
			// Force reserved bits to zero (as per MQTT 5 spec)
			options.setFlagValue(SubscribeOptionsFlags::RESERVED, 0);
		}

		std::size_t Subscription::getEncodedBytesSize() const noexcept
		{
			return topicFilter.encodingSize() + sizeof(options);
		}

		void Subscription::encode(ByteBuffer& buffer) const
		{
			topicFilter.encode(buffer);
			buffer += options.getFlags();
		}
	}
}