// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribeAck::UnSubscribeAck(UnSubscribeAckVariableHeader&& variableHeader, UnSubscribeAckPayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_UnSubscribeAckFixedHeaderFlags)),
			  m_variableHeader(std::move(variableHeader)),
			  m_payloadHeader(std::move(payloadHeader))
		{
			setUpHeaders();
		}

		UnSubscribeAck::UnSubscribeAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		UnSubscribeAck::UnSubscribeAck(UnSubscribeAck&& other) noexcept
			: BasePacket(std::move(other)),
			  m_variableHeader(std::move(other.m_variableHeader)),
			  m_payloadHeader(std::move(other.m_payloadHeader))
		{
			setUpHeaders();
		}

		UnSubscribeAck::~UnSubscribeAck()
		{
		}

		UnSubscribeAck& UnSubscribeAck::operator=(UnSubscribeAck&& other) noexcept
		{
			if (this != &other)
			{
				BasePacket::operator=(std::move(other));
				m_variableHeader = std::move(other.m_variableHeader);
				m_payloadHeader = std::move(other.m_payloadHeader);
				setUpHeaders();
			}
			return *this;
		}

		PacketType UnSubscribeAck::getPacketType() const noexcept
		{
			return PacketType::UNSUBSCRIBE_ACKNOWLEDGE;
		}

		const UnSubscribeAckVariableHeader& UnSubscribeAck::getVariableHeader() const
		{
			return m_variableHeader;
		}

		const UnSubscribeAckPayloadHeader& UnSubscribeAck::getPayloadHeader() const
		{
			return m_payloadHeader;
		}

		void UnSubscribeAck::setUpHeaders() noexcept
		{
			addDecodeHeader(&m_variableHeader);
			addDecodeHeader(&m_payloadHeader);
		}
	}
}
