// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CODES_CONNECTREASONCODE_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CODES_CONNECTREASONCODE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class ConnectReasonCode : std::uint8_t
		{
			SUCCESS = 0U,                             //The Connection is accepted.
			UNSPECIFIED_ERROR = 128U,                 //The Server does not wish to reveal the reason for the failure, or none of the other Reason Codes apply.
			MALFORMED_PACKET = 129U,                  //Data within the CONNECT packet could not be correctly parsed.
			PROTOCOL_ERROR = 130U,                    //Data in the CONNECT packet does not conform to this specification.
			IMPLEMENTATION_SPECIFIC_ERROR = 131U,     //The CONNECT is valid but is not accepted by this Server.
			UNSUPPORTED_PROTOCOL_VERSION = 132U,      //The Server does not support the version of the MQTT protocol requested by the Client.
			CLIENT_IDENTIFIER_NOT_VALID = 133U,       //The Client Identifier is a valid string but is not allowed by the Server.
			BAD_USER_NAME_OR_PASSWORD = 134U,         //The Server does not accept the User Name or Password specified by the Client.
			NOT_AUTHORIZED = 135U,                    //The Client is not authorized to connect.
			SERVER_UNAVAILABLE = 136U,                //The MQTT Server is not available.
			SERVER_BUSY = 137U,                       //The Server is busy. Try again later.
			BANNED = 138U,                            //This Client has been banned by administrative action. Contact the server administrator.
			BAD_AUTHENTICATION_METHOD = 140U,         //The authentication method is not supported or does not match the authentication method currently in use.
			TOPIC_NAME_INVALID = 144U,                //The Will Topic Name is not malformed, but is not accepted by this Server.
			PACKET_TOO_LARGE = 149U,                  //The CONNECT packet exceeded the maximum permissible size.
			QUOTA_EXCEEDED = 151U,                    //An implementation or administrative imposed limit has been exceeded.
			PAYLOAD_FORMAT_INVALID = 153U,            //The Will Payload does not match the specified Payload Format Indicator.
			RETAIN_NOT_SUPPORTED = 154U,              //The Server does not support retained messages, and Will Retain was set to 1.
			QOS_NOT_SUPPORTED = 155U,                 //The Server does not support the QoS set in Will QoS.
			USE_ANOTHER_SERVER = 156U,                //The Client should temporarily use another server.
			SERVER_MOVED = 157U,                      //The Client should permanently use another server.
			CONNECTION_RATE_EXCEEDED = 159U           //The connection rate limit has been exceeded.
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_CONNECT_CODES_CONNECTREASONCODE_H
