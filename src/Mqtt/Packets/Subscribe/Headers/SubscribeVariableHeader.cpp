// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		SubscribeVariableHeader::SubscribeVariableHeader() noexcept
			: packetId(0), properties()
		{
		}

		SubscribeVariableHeader::SubscribeVariableHeader(std::uint16_t packetId, Properties&& properties) noexcept
			: packetId(packetId), properties(std::move(properties))
		{
		}

		void SubscribeVariableHeader::encode(ByteBuffer& buffer) const
		{
			buffer.append(packetId);
			properties.encode(buffer);
		}

		std::size_t SubscribeVariableHeader::getEncodedBytesSize() const noexcept
		{
			return sizeof(packetId) + properties.encodingSize();
		}
	}
}