// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/PublishRec.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PublishRec::PublishRec(PubRecVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(0U)),
			  m_variableHeader{ std::move(variableHeader) }
		{
			setUpHeaders();
		}

		PublishRec::PublishRec(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishRec::PublishRec(PublishRec&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(std::move(other.m_variableHeader))
		{
			setUpHeaders();
		}

		PublishRec::~PublishRec()
		{
		}

		PublishRec& PublishRec::operator=(PublishRec&& other) noexcept
		{
			if (this != &other)
			{
				BasePacket::operator=(std::move(other));
				m_variableHeader = std::move(other.m_variableHeader);
				setUpHeaders();
			}
			return *this;
		}

		PacketType PublishRec::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_RECEIVED;
		}

		const PubRecVariableHeader& PublishRec::getVariableHeader() const
		{
			return m_variableHeader;
		}

		void PublishRec::setUpHeaders() noexcept
		{
			addEncodeHeader(&m_variableHeader);
			addDecodeHeader(&m_variableHeader);
		}

		void PublishRec::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBREC fixed header
		}
	}
}
