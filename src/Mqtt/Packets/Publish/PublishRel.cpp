// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/PublishRel.h>

namespace kmMqtt
{
	namespace mqtt
	{
		//FixedHeaderFlags is 2 for PUBREL packets as per MQTT 5 spec (Reserved as 0010)
		PublishRel::PublishRel(PubRelVariableHeader&& variableHeader) noexcept
			: BasePacket(FixedHeaderFlags(2U)), m_variableHeader{ new PubRelVariableHeader(std::move(variableHeader)) }
		{
			setUpHeaders();
		}

		PublishRel::PublishRel(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		PublishRel::PublishRel(PublishRel&& other) noexcept
			: BasePacket(std::move(other)),
			m_variableHeader(other.m_variableHeader)
		{
			other.m_variableHeader = nullptr;
		}

		PublishRel::~PublishRel()
		{
			delete m_variableHeader;
		}

		PacketType PublishRel::getPacketType() const noexcept
		{
			return PacketType::PUBLISH_RELEASED;
		}

		const PubRelVariableHeader& PublishRel::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		void PublishRel::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new PubRelVariableHeader();
			}

			addEncodeHeader(m_variableHeader);
			addDecodeHeader(m_variableHeader);
		}

		void PublishRel::onFixedHeaderDecoded() const
		{
			// No additional logic required for PUBREC fixed header
		}
	}
}