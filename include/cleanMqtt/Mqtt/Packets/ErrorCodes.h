#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_ERRORCODES_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_ERRORCODES_H

#include <cstdint>
#include <cleanMqtt/Mqtt/Packets/Connection/Codes/DisconnectReasonCode.h>
#include <cleanMqtt/Mqtt/Packets/PacketType.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			enum class DecodeErrorCode : std::uint8_t
			{
				NO_ERROR = 0U,
				UNSPECIFIED_ERROR,
				MALFORMED_PACKET,
				PROTOCOL_ERROR,
				INTERNAL_ERROR
			};

			struct DecodeResult
			{
				DecodeResult() = default;
				DecodeResult(DecodeErrorCode errorCode, std::string errorReason = "") noexcept
					: code{ errorCode }, reason{ errorReason }
				{
				}

				PacketType packetType{ PacketType::RESERVED };
				DecodeErrorCode code{ DecodeErrorCode::NO_ERROR };
				std::string reason;

				inline bool isSuccess() const noexcept
				{
					return code == DecodeErrorCode::NO_ERROR;
				}
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_ERRORCODES_H  
