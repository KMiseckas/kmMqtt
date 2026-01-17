// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/UnSubscribe/Headers/UnSubscribeAckVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		DecodeResult UnSubscribeAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			packetId = buffer.readUInt16();

			if (packetId == 0)
			{
				result.code = DecodeErrorCode::PROTOCOL_ERROR;
				result.reason = "Packet ID cannot be zero in UNSUBACK";
				return result;
			}

			if (buffer.readHeadroom() > 0)
			{
				result = properties.decode(buffer);
			}

			return result;
		}
	}
}