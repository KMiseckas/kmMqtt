#ifndef INCLUDE_CLEANMQTT_MQTT_CLIENTERRORCODE_H
#define INCLUDE_CLEANMQTT_MQTT_CLIENTERRORCODE_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class ClientErrorCode : std::uint16_t
		{
			No_Error = 0U, //No error occurred.
			Unknown = 1U,

			Invalid_Argument = 2U,
			Missing_Argument = 3U,
			Not_Connected = 4U,
			Socket_Error = 5U,
			MQTT_Not_Active = 6U,
			Failed_Sending_Packet = 7U,
			Failed_Writing_To_Persistent_Storage = 8U, //Not used yet
			Failed_Decoding_Packet = 9U,
			TimeOut = 8U,
			Using_Tick_Async = 9U, //Client is using async tick mode, cannot call tick functions explicitly.

			//Connect Errors
			ConnectError = 1000U,
			Already_Connected = 1001U,
			Already_Connecting = 1002U,
			Socket_Connect_Failed = 1003U,
			Server_Session_Present_Mismatch = 1004U, //Server returned a session present property but client has no previous session state.

			//Disconnect Errors
			DisconnectError = 2000U,
			Already_Disconnected = 2001U,

			//Publish Errors
			PublishError = 3000U,
			Recv_Topic_Alias_Disallowed = 3001U, //Client received a topic alias but has requested topic aliasing to be disabled on connect.
			Recv_Invalid_Topic_Alias = 3002U, //Client received a topic alias that is out of range (e.g. 0 or greater than maximum allowed).
			Recv_Empty_Topic_Name_And_Invalid_Alias = 3003U, //Client allows topic aliases, but received a publish with an empty topic name and invalid topic alias.

			//Subscribe Errors
			SubscribeError = 4000U,
			SubscribeIdentifiersNotSupported = 4001U, //Client tried to subscribe with subscribe identifier but broker does not support subscribe identifiers.

			//Unscubscribe Errors
			UnsubscribeError = 5000U,

		};
	}
}

#endif
