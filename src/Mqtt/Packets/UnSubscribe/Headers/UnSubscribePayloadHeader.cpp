// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribePayloadHeader.h"

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribePayloadHeader::UnSubscribePayloadHeader(std::vector<UTF8String>&& topics) noexcept
			: topics(std::move(topics)) {
		}

		void UnSubscribePayloadHeader::encode(ByteBuffer& buffer) const
		{
			for (const auto& topic : topics)
			{
				topic.encode(buffer);
			}
		}

		std::size_t UnSubscribePayloadHeader::getEncodedBytesSize() const noexcept
		{
			std::size_t size = 0;
			for (const auto& topic : topics)
			{
				size += topic.encodingSize();
			}
			return size;
		}
	}
}
