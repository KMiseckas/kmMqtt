#ifndef INCLUDE_KMMQTT_MQTT_CLIENTERROR_H
#define INCLUDE_KMMQTT_MQTT_CLIENTERROR_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Enums/ClientErrorCode.h>
#include <cstring>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API ClientError
		{
			ClientError() = default;
			ClientError(const ClientErrorCode code)
				: errorCode{ code } {}
			ClientError(const ClientErrorCode code, const char* msg)
				: errorCode{code}, errorMsg{msg}{}

			ClientErrorCode errorCode{ ClientErrorCode::No_Error };
			std::string errorMsg{ "" };

			constexpr bool noError() const noexcept { return errorCode == ClientErrorCode::No_Error; }
			
			constexpr bool operator==(ClientErrorCode otherCode) const noexcept { return errorCode == otherCode; }
			constexpr bool operator!=(ClientErrorCode otherCode) const noexcept { return errorCode != otherCode; }
		};

		constexpr bool operator==(ClientErrorCode errorCode, const ClientError& clientError) {return clientError == errorCode;}
		constexpr bool operator!=(ClientErrorCode errorCode, const ClientError& clientError) { return clientError != errorCode; }
	}
}

#endif
