// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Subscribe/Subscribe.h>

namespace kmMqtt
{
	namespace mqtt
	{
		Subscribe::Subscribe(SubscribeVariableHeader&& variableHeader, SubscribePayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_SubscribeFixedHeaderFlags)),
			m_variableHeader(new SubscribeVariableHeader(std::move(variableHeader))),
			m_payloadHeader(new SubscribePayloadHeader(std::move(payloadHeader)))
		{
			setUpHeaders();
		}

		Subscribe::Subscribe(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Subscribe::Subscribe(Subscribe&& other) noexcept
			: BasePacket{ std::move(other) },
			m_variableHeader{ other.m_variableHeader },
			m_payloadHeader{ other.m_payloadHeader }
		{
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;
		}

		Subscribe::~Subscribe()
		{
			delete m_payloadHeader;
			delete m_variableHeader;
		}

		PacketType Subscribe::getPacketType() const noexcept
		{
			return PacketType::SUBSCRIBE;
		}

		const SubscribeVariableHeader& Subscribe::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const SubscribePayloadHeader& Subscribe::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void Subscribe::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new SubscribeVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new SubscribePayloadHeader();
			}

			addEncodeHeader(m_variableHeader);
			addEncodeHeader(m_payloadHeader);
		}
	}
}