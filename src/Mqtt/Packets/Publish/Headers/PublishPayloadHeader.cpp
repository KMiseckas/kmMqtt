// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/Headers/PublishPayloadHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PublishPayloadHeader::PublishPayloadHeader() noexcept
		{
		}

		PublishPayloadHeader::PublishPayloadHeader(ByteBuffer&& payload) noexcept
			: payload{ std::move(payload) }
		{
		}

		DecodeResult PublishPayloadHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			payload.expand(buffer.readHeadroom());
			payload.append(buffer.bytes() + buffer.readCursor(), payload.capacity());
			buffer.incrementReadCursor(buffer.readHeadroom());

			return result;
		}

		void PublishPayloadHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(payload);
		}

		std::size_t PublishPayloadHeader::getEncodedBytesSize() const noexcept
		{
			return payload.size();
		}
	}
}