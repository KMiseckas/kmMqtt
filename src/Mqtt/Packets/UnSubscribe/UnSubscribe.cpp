// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribe.h>

namespace kmMqtt
{
	namespace mqtt
	{
		UnSubscribe::UnSubscribe(UnSubscribeVariableHeader&& variableHeader, UnSubscribePayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_UnSubscribeFixedHeaderFlags)),
			m_variableHeader(new UnSubscribeVariableHeader(std::move(variableHeader))),
			m_payloadHeader(new UnSubscribePayloadHeader(std::move(payloadHeader)))
		{
			setUpHeaders();
		}

		UnSubscribe::UnSubscribe(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		UnSubscribe::UnSubscribe(UnSubscribe&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(other.m_variableHeader),
			m_payloadHeader(other.m_payloadHeader)
		{
			other.m_variableHeader = nullptr;
			other.m_payloadHeader = nullptr;
		}

		UnSubscribe::~UnSubscribe()
		{
			delete m_variableHeader;
			delete m_payloadHeader;
		}

		PacketType UnSubscribe::getPacketType() const noexcept
		{
			return PacketType::UNSUBSCRIBE;
		}

		const UnSubscribeVariableHeader& UnSubscribe::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		const UnSubscribePayloadHeader& UnSubscribe::getPayloadHeader() const
		{
			return *m_payloadHeader;
		}

		void UnSubscribe::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new UnSubscribeVariableHeader();
			}

			if (m_payloadHeader == nullptr)
			{
				m_payloadHeader = new UnSubscribePayloadHeader();
			}

			addEncodeHeader(m_variableHeader);
			addEncodeHeader(m_payloadHeader);
		}
	}
}