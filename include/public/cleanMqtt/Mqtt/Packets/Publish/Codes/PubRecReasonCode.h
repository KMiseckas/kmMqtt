#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRECREASONCODE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRECREASONCODE_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class PubRecReasonCode : std::uint8_t
		{
			SUCCESS = 0U,                          // The message is accepted. Publication of the QoS 2 message proceeds.
			NO_MATCHING_SUBSCRIBERS = 16U,         // The message is accepted but there are no subscribers. This is sent only by the Server.
			UNSPECIFIED_ERROR = 128U,              // The receiver does not accept the publish but either does not want to reveal the reason.
			IMPLEMENTATION_SPECIFIC_ERROR = 131U,  // The PUBLISH is valid but the receiver is not willing to accept it.
			NOT_AUTHORIZED = 135U,                 // The PUBLISH is not authorized.
			TOPIC_NAME_INVALID = 144U,             // The Topic Name is not malformed, but is not accepted by this Client or Server.
			PACKET_IDENTIFIER_IN_USE = 145U,       // The Packet Identifier is already in use. 
			QUOTA_EXCEEDED = 151U,                 // An implementation or administrative imposed limit has been exceeded.
			PAYLOAD_FORMAT_INVALID = 153U          // The payload format does not match the one specified in the Payload Format Indicator.
		};
	}
}

#endif // INCLUDE_CLEANMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBRECREASONCODE_H
