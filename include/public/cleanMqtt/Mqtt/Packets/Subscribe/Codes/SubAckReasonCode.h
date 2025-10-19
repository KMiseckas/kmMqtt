#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_CODES_SUBACKREASONCODE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_CODES_SUBACKREASONCODE_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class SubAckReasonCode : std::uint8_t
		{
			GRANTED_QOS_0 = 0x00,                        // The subscription is accepted and the maximum QoS sent will be QoS 0. This might be a lower QoS than was requested.
			GRANTED_QOS_1 = 0x01,                        // The subscription is accepted and the maximum QoS sent will be QoS 1. This might be a lower QoS than was requested.
			GRANTED_QOS_2 = 0x02,                        // The subscription is accepted and any received QoS will be sent to this subscription.
			UNSPECIFIED_ERROR = 0x80,                    // The subscription is not accepted and the Server either does not wish to reveal the reason or none of the other Reason Codes apply.
			IMPLEMENTATION_SPECIFIC_ERROR = 0x83,        // The SUBSCRIBE is valid but the Server does not accept it.
			NOT_AUTHORIZED = 0x87,                       // The Client is not authorized to make this subscription.
			TOPIC_FILTER_INVALID = 0x8F,                 // The Topic Filter is correctly formed but is not allowed for this Client.
			PACKET_IDENTIFIER_IN_USE = 0x91,             // The specified Packet Identifier is already in use.
			QUOTA_EXCEEDED = 0x97,                       // An implementation or administrative imposed limit has been exceeded.
			SHARED_SUBSCRIPTIONS_NOT_SUPPORTED = 0x9E,   // The Server does not support Shared Subscriptions for this Client.
			SUBSCRIPTION_IDENTIFIERS_NOT_SUPPORTED = 0xA1,// The Server does not support Subscription Identifiers; the subscription is not accepted.
			WILDCARD_SUBSCRIPTIONS_NOT_SUPPORTED = 0xA2   // The Server does not support Wildcard Subscriptions; the subscription is not accepted.
		};
	}
}

#endif // INCLUDE_CLEANMQTT_MQTT_PACKETS_SUBSCRIBE_CODES_SUBACKREASONCODE_H
