// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/Transport/Jobs/PingComposer.h>
#include <kmMqtt/Mqtt/PacketHelper.h>

namespace kmMqtt
{
	namespace mqtt
	{
		ComposeResult PingComposer::compose() noexcept
		{
			auto packet{ createPingRequestPacket() };
			auto result{ packet.encode() };

			return ComposeResult{ result, packet.extractDataBuffer() };
		}

		void PingComposer::cancel() noexcept
		{
		}
	}
}
