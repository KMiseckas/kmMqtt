// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		SubscribeAck::SubscribeAck(SubscribeAckVariableHeader&& variableHeader, SubscribeAckPayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_SubscribeAckFixedHeaderFlags)),
			  m_variableHeader(std::move(variableHeader)),
			  m_payloadHeader(std::move(payloadHeader))
		{
			setUpHeaders();
		}

		SubscribeAck::SubscribeAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		SubscribeAck::SubscribeAck(SubscribeAck&& other) noexcept
			: BasePacket(std::move(other)),
			  m_variableHeader(std::move(other.m_variableHeader)),
			  m_payloadHeader(std::move(other.m_payloadHeader))
		{
			setUpHeaders();
		}

		SubscribeAck::~SubscribeAck()
		{
		}

		PacketType SubscribeAck::getPacketType() const noexcept
		{
			return PacketType::SUBSCRIBE_ACKNOWLEDGE;
		}

		const SubscribeAckVariableHeader& SubscribeAck::getVariableHeader() const
		{
			return m_variableHeader;
		}

		const SubscribeAckPayloadHeader& SubscribeAck::getPayloadHeader() const
		{
			return m_payloadHeader;
		}

		void SubscribeAck::setUpHeaders() noexcept
		{
			addDecodeHeader(&m_variableHeader);
			addDecodeHeader(&m_payloadHeader);
		}
	}
}
