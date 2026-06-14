// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/PublishAck.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PublishAck::PublishAck(PubAckVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(0U)), m_variableHeader{ std::move(variableHeader) }
		{
			setUpHeaders();
		}

		PublishAck::PublishAck(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishAck::PublishAck(PublishAck&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(std::move(other.m_variableHeader))
		{
			setUpHeaders();
		}

		PublishAck::~PublishAck()
		{
		}

		PublishAck& PublishAck::operator=(PublishAck&& other) noexcept
		{
			if (this != &other)
			{
				BasePacket::operator=(std::move(other));
				m_variableHeader = std::move(other.m_variableHeader);
				setUpHeaders();
			}
			return *this;
		}

		PacketType PublishAck::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_ACKNOWLEDGE;
		}

		const PubAckVariableHeader& PublishAck::getVariableHeader() const
		{
			return m_variableHeader;
		}

		void PublishAck::setUpHeaders() noexcept
		{
			addEncodeHeader(&m_variableHeader);
			addDecodeHeader(&m_variableHeader);
		}

		void PublishAck::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBACK fixed header
		}
	}
}
