// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		SubscribeAck::SubscribeAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		SubscribeAck::SubscribeAck(SubscribeAck&& other) noexcept
			: BasePacket(std::move(other))
		{
			m_variableHeader = other.m_variableHeader;
			m_payloadHeader = other.m_payloadHeader;
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;

			setUpHeaders();
		}

		SubscribeAck::~SubscribeAck()
		{
			delete m_variableHeader;
			delete m_payloadHeader;
		}

		PacketType SubscribeAck::getPacketType() const noexcept
		{
			return PacketType::SUBSCRIBE_ACKNOWLEDGE;
		}

		const SubscribeAckVariableHeader& SubscribeAck::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const SubscribeAckPayloadHeader& SubscribeAck::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void SubscribeAck::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new SubscribeAckVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new SubscribeAckPayloadHeader();
			}

			addDecodeHeader(m_variableHeader);
			addDecodeHeader(m_payloadHeader);
		}
	}
}