// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Connection/ConnectAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ConnectAck::ConnectAck() noexcept
			: BasePacket(FixedHeaderFlags(k_ConnectAckFixedHeaderFlags))
		{
			setUpHeaders();
		}

		ConnectAck::ConnectAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		ConnectAck::ConnectAck(ConnectAck&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(std::move(other.m_variableHeader))
		{
			setUpHeaders();
		}

		ConnectAck::~ConnectAck()
		{
		}

		ConnectAck& ConnectAck::operator=(ConnectAck&& other) noexcept
		{
			if (this != &other)
			{
				BasePacket::operator=(std::move(other));
				m_variableHeader = std::move(other.m_variableHeader);
				setUpHeaders();
			}
			return *this;
		}

		PacketType ConnectAck::getPacketType() const noexcept
		{
			return PacketType::CONNECT_ACKNOWLEDGE;
		}

		const ConnectAckVariableHeader& ConnectAck::getVariableHeader() const noexcept
		{
			return m_variableHeader;
		}

		void ConnectAck::setUpHeaders() noexcept
		{
			addDecodeHeader(&m_variableHeader);
		}
	}
}
