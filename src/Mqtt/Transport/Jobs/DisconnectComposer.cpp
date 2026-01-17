// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Transport/Jobs/DisconnectComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult DisconnectComposer::compose() noexcept
		{
			Disconnect packet{ createDisconnectPacket(*m_mqttConnectionInfo, m_options, m_reasonCode) };
			EncodeResult result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void DisconnectComposer::cancel() noexcept
		{
			// No specific cancellation logic for Connect job
		}
	}
}
