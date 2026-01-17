#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBCOMPREASONCODE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBCOMPREASONCODE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class PubCompReasonCode : std::uint8_t
		{
			SUCCESS = 0U,                          // Packet Identifier released. Publication of QoS 2 message is complete.
			PACKET_IDENTIFIER_NOT_FOUND = 146U,    // The Packet Identifier is not known. This is not an error during recovery, but at other times indicates a mismatch between the Session State on the Client and Server.
		};
	}
}

#endif // INCLUDE_KMMQTT_MQTT_PACKETS_PUBLISH_CODES_PUBCOMPREASONCODE_H
