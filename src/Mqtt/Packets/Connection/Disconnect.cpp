// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Packets/Connection/Disconnect.h>

namespace kmMqtt
{
	namespace mqtt
	{
		Disconnect::Disconnect(DisconnectVariableHeader&& varHeader) noexcept
			: BasePacket(FixedHeaderFlags(k_DisconnectFixedHeaderFlags)),
			m_variableHeader(new DisconnectVariableHeader(std::move(varHeader)))
		{
			setUpHeaders();
		}

		Disconnect::Disconnect(ByteBuffer&& dataBuffer) noexcept
			: BasePacket(std::move(dataBuffer))
		{
			setUpHeaders();
		}

		Disconnect::Disconnect(Disconnect&& other) noexcept
			: BasePacket{ std::move(other) },
			m_variableHeader{ other.m_variableHeader }
		{
			other.m_variableHeader = nullptr;
		}

		Disconnect::~Disconnect()
		{
			delete m_variableHeader;
		}

		PacketType Disconnect::getPacketType() const noexcept
		{
			return PacketType::DISCONNECT;
		}

		const DisconnectVariableHeader& Disconnect::getVariableHeader() const
		{
			return *m_variableHeader;
		}

		void Disconnect::setUpHeaders() noexcept
		{
			if (m_variableHeader == nullptr)
			{
				m_variableHeader = new DisconnectVariableHeader();
			}

			addDecodeHeader(m_variableHeader);
			addEncodeHeader(m_variableHeader);
		}
	}
}