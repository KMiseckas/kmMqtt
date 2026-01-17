#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_CODES_UNSUBACKREASONCODE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_CODES_UNSUBACKREASONCODE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class UnSubAckReasonCode : std::uint8_t
		{
			SUCCESS = 0x00,                         // The subscription is deleted.
			NO_SUBSCRIPTION_EXISTED = 0x11,         // No matching Topic Filter is being used by the Client.
			UNSPECIFIED_ERROR = 0x80,               // The unsubscribe could not be completed and the Server either does not wish to reveal the reason or none of the other Reason Codes apply.
			IMPLEMENTATION_SPECIFIC_ERROR = 0x83,   // The UNSUBSCRIBE is valid but the Server does not accept it.
			NOT_AUTHORIZED = 0x87,                  // The Client is not authorized to unsubscribe.
			TOPIC_FILTER_INVALID = 0x8F,            // The Topic Filter is correctly formed but is not allowed for this Client.
			PACKET_IDENTIFIER_IN_USE = 0x91         // The specified Packet Identifier is already in use.
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_PACKETS_UNSUBSCRIBE_CODES_UNSUBACKREASONCODE_H
