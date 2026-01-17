// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribePayloadHeader.h"

namespace kmMqtt
{
	namespace mqtt
	{
		SubscribePayloadHeader::SubscribePayloadHeader(std::vector<Subscription>&& subscriptions) noexcept
			: subscriptions(std::move(subscriptions)) {
		}

		void SubscribePayloadHeader::encode(ByteBuffer& buffer) const
		{
			for (const auto& sub : subscriptions)
			{
				sub.encode(buffer);
			}
		}

		std::size_t SubscribePayloadHeader::getEncodedBytesSize() const noexcept
		{
			std::size_t size = 0;
			for (const auto& sub : subscriptions)
			{
				size += sub.getEncodedBytesSize();
			}
			return size;
		}
	}
}