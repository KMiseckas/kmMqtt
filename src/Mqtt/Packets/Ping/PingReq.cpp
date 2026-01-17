// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Ping/PingReq.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PingReq::PingReq() noexcept
			: BasePacket(FixedHeaderFlags(k_PingReqFixedHeaderFlags))
		{
		}

		PingReq::PingReq(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
		}

		PingReq::PingReq(PingReq&& other) noexcept
			: BasePacket{ std::move(other) }
		{
		}

		PingReq::~PingReq()
		{
		}

		PacketType PingReq::getPacketType() const noexcept
		{
			return PacketType::PING_REQUQEST;
		}
	}
}