// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBACKREASONCODE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBACKREASONCODE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class PubAckReasonCode : std::uint8_t
		{
			SUCCESS = 0U,                          // The message is accepted. Publication of the QoS 1 message proceeds.
			NO_MATCHING_SUBSCRIBERS = 16U,         // The message is accepted but there are no subscribers.
			UNSPECIFIED_ERROR = 128U,              // The receiver does not accept the publish but does not want to reveal the reason.
			IMPLEMENTATION_SPECIFIC_ERROR = 131U,  // The PUBLISH is valid but the receiver is not willing to accept it.
			NOT_AUTHORIZED = 135U,                 // The PUBLISH is not authorized.
			TOPIC_NAME_INVALID = 144U,             // The Topic Name is not malformed, but is not accepted.
			PACKET_IDENTIFIER_IN_USE = 145U,       // The Packet Identifier is already in use.
			QUOTA_EXCEEDED = 151U,                 // An implementation or administrative imposed limit has been exceeded.
			PAYLOAD_FORMAT_INVALID = 153U          // The payload format does not match the specified Payload Format Indicator.
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBACKREASONCODE_H
