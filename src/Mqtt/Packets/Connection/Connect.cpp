// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Connection/Connect.h>

namespace kmMqtt
{
	namespace mqtt
	{
		Connect::Connect(ConnectVariableHeader&& variableHeader, ConnectPayloadHeader&& payloadHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_ConnectFixedHeaderFlags)),
			  m_variableHeader(std::move(variableHeader)),
			  m_payloadHeader(std::move(payloadHeader))
		{
			setUpHeaders();
		}

		Connect::Connect(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Connect::Connect(Connect&& other) noexcept
			: BasePacket{ std::move(other) },
			m_variableHeader{ std::move(other.m_variableHeader) },
			m_payloadHeader{ std::move(other.m_payloadHeader) }
		{
			setUpHeaders();
		}

		Connect::Connect() noexcept
			: BasePacket(FixedHeaderFlags(k_ConnectFixedHeaderFlags))
		{
			setUpHeaders();
		}

		Connect::~Connect()
		{
		}

		PacketType Connect::getPacketType() const noexcept
		{
			return PacketType::CONNECT;
		}

		const ConnectVariableHeader& Connect::getVariableHeader() const
		{
			return m_variableHeader;
		}

		const ConnectPayloadHeader& Connect::getPayloadHeader() const
		{
			return m_payloadHeader;
		}

		void Connect::setUpHeaders() noexcept
		{
			addEncodeHeader(&m_variableHeader);
			addEncodeHeader(&m_payloadHeader);
		}
	}
}
