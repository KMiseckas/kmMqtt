// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Connection/Headers/ConnectAckVariableHeader.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ConnectAckVariableHeader::ConnectAckVariableHeader() noexcept
		{
			flags.setFlagValue(ConnectAcknowledgeFlags::RESERVED, 0);
		}

		DecodeResult ConnectAckVariableHeader::decode(const ByteBuffer& buffer) noexcept
		{
			DecodeResult result;

			try
			{
				flags.overrideFlags(buffer.readUint8());
				reasonCode = static_cast<ConnectReasonCode>(buffer.readUint8());
			}
			catch (const std::out_of_range& e)
			{
				return DecodeResult(DecodeErrorCode::MALFORMED_PACKET, e.what());
			}

			result = properties.decode(buffer);

			return result;
		}
	}
}