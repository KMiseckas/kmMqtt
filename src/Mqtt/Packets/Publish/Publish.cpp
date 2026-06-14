// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Publish/Publish.h>

namespace kmMqtt
{
	namespace mqtt
	{
		Publish::Publish(PublishPayloadHeader&& payloadHeader, PublishVariableHeader&& variableHeader, const EncodedPublishFlags& flags) noexcept
			:BasePacket(flags),
			m_payloadHeader{ std::move(payloadHeader) },
			m_variableHeader{ std::move(variableHeader) }
		{
			setUpHeaders();
		}

		Publish::Publish(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Publish::Publish(Publish&& other) noexcept
			: BasePacket(std::move(other)),
			m_payloadHeader(std::move(other.m_payloadHeader)),
			m_variableHeader(std::move(other.m_variableHeader))
		{
			setUpHeaders();
		}

		Publish::~Publish()
		{
		}

		PacketType Publish::getPacketType() const noexcept
		{
			return PacketType::PUBLISH;
		}

		const PublishVariableHeader& Publish::getVariableHeader() const
		{
			return m_variableHeader;
		}

		const PublishPayloadHeader& Publish::getPayloadHeader() const
		{
			return m_payloadHeader;
		}

		void Publish::setUpHeaders() noexcept
		{
			addEncodeHeader(&m_variableHeader);
			addEncodeHeader(&m_payloadHeader);

			addDecodeHeader(&m_variableHeader);
			addDecodeHeader(&m_payloadHeader);
		}

		void Publish::onFixedHeaderDecoded() const
		{
			m_variableHeader.qos = static_cast<Qos>(getFixedHeader().flags.getFlagValue(static_cast<std::uint8_t>(PublishFlags::QOS)));
		}
	}
}
