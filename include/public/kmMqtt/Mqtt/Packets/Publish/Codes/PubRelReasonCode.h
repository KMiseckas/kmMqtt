// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRELREASONCODE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRELREASONCODE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class PubRelReasonCode : std::uint8_t
		{
			SUCCESS = 0U,                          // Message released.
			PACKET_IDENTIFIER_NOT_FOUND = 146U,    // The Packet Identifier is not known. This is not an error during recovery, but at other times indicates a mismatch between the Session State on the Client and Server.
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRELREASONCODE_H
