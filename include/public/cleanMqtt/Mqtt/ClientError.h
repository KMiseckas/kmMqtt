#ifndef INCLUDE_CLEANMQTT_MQTT_CLIENTERROR_H
#define INCLUDE_CLEANMQTT_MQTT_CLIENTERROR_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Enums/ClientErrorCode.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API ClientError
		{
			constexpr ClientError() = default;
			constexpr ClientError(const ClientErrorCode code)
				: errorCode{ code } {}
			constexpr ClientError(const ClientErrorCode code, const char* msg)
				: errorCode{code}, errorMsg{msg}{}

			ClientErrorCode errorCode{ ClientErrorCode::No_Error };
			const char* errorMsg{ "" };

			constexpr bool noError() const noexcept { return errorCode == ClientErrorCode::No_Error; }
			
			constexpr bool operator==(ClientErrorCode otherCode) const noexcept { return errorCode == otherCode; }
			constexpr bool operator!=(ClientErrorCode otherCode) const noexcept { return errorCode != otherCode; }
		};

		constexpr bool operator==(ClientErrorCode errorCode, const ClientError& clientError) {return clientError == errorCode;}
		constexpr bool operator!=(ClientErrorCode errorCode, const ClientError& clientError) { return clientError != errorCode; }
	}
}

#endif
