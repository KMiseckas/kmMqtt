// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Transport/Jobs/PubRelComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult PubRelComposer::compose() noexcept
		{
			PublishRel packet{ createPubRelPacket(m_publishPacketId, m_reasonCode, m_options) };
			EncodeResult result{ packet.encode() };
			result.packetId = m_publishPacketId;

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PubRelComposer::cancel() noexcept
		{
		}
	}
}
