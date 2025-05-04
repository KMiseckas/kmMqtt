#ifndef INCLUDE_CLEANMQTT_MQTT_CLIENTERRORCODE_H
#define INCLUDE_CLEANMQTT_MQTT_CLIENTERRORCODE_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class ClientErrorCode : std::uint16_t
		{
			No_Error = 0U,
			Unknown = 1U,

			Invalid_Argument = 2U,
			Missing_Argument = 3U,
			Not_Connected = 4U,
			Socket_Error = 5U,
			MQTT_Not_Active = 6U,

			//Connect Errors
			ConnectError = 1000U,
			Already_Connected = 1001U,
			Already_Connecting = 1002U,
			Socket_Connect_Failed = 1003U,

			//Disconnect Errors
			DisconnectError = 2000U,
			Already_Disconnected = 2001U,

			//Publish Errors
			PublishError = 3000U,

			//Subscribe Errors
			SubscribeError = 4000U,

			//Unscubscribe Errors
			UnsubscribeError = 5000U,

		};
	}
}

#endif
