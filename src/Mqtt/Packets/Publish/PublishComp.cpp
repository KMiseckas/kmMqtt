// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/PublishComp.h>

namespace kmMqtt
{
	namespace mqtt
	{
		PublishComp::PublishComp(PubCompVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(0U)), m_variableHeader{ new PubCompVariableHeader(std::move(variableHeader)) }
		{
			setUpHeaders();
		}

		PublishComp::PublishComp(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishComp::PublishComp(PublishComp&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(other.m_variableHeader)
		{
			other.m_variableHeader = nullptr;
		}

		PublishComp::~PublishComp()
		{
			delete m_variableHeader;
		}

		PacketType PublishComp::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_COMPLETE;
		}

		const PubCompVariableHeader& PublishComp::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		void PublishComp::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new PubCompVariableHeader();
			}

			addEncodeHeader(m_variableHeader);
			addDecodeHeader(m_variableHeader);
		}

		void PublishComp::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBREC fixed header
		}
	}
}