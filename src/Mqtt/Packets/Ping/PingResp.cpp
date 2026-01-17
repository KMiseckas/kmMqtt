// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Ping/PingResp.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PingResp::PingResp() noexcept
			: BasePacket(FixedHeaderFlags(k_PingRespFixedHeaderFlags))
		{
		}

		PingResp::PingResp(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
		}

		PingResp::PingResp(PingResp&& other) noexcept
			: BasePacket{ std::move(other) }
		{
		}

		PingResp::~PingResp()
		{
		}

		PacketType PingResp::getPacketType() const noexcept
		{
			return PacketType::PING_RESPONSE;
		}
	}
}