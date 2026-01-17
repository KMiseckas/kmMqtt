// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Subscribe/Headers/SubscribeAckPayloadHeader.h>
#include <kmMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h>

namespace kmMqtt
{
	namespace mqtt
	{
		DecodeResult SubscribeAckPayloadHeader::decode(const ByteBuffer& buffer) noexcept
		{
			reasonCodes.clear();

			while (buffer.readHeadroom() > 0)
			{
				reasonCodes.push_back(static_cast<SubAckReasonCode>(buffer.readUint8()));
			}

			return DecodeResult{ DecodeErrorCode::NO_ERROR };
		}
	}
}