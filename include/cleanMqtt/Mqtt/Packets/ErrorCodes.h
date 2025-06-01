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

			enum class EncodeErrorCode : std::uint8_t
			{
				NO_ERROR = 0U,
				INTERNAL_ERROR
				//TODO : Add more error codes for encoding errors, currently only internal error is defined. See BasePacket::encode() for where its being set.
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

				DisconnectReasonCode getDisconnectReason() const noexcept
				{
					switch (code)
					{
					case DecodeErrorCode::PROTOCOL_ERROR:
						return DisconnectReasonCode::PROTOCOL_ERROR;
					case DecodeErrorCode::INTERNAL_ERROR:
						return DisconnectReasonCode::IMPLEMENTATION_SPECIFIC_ERROR;
					case DecodeErrorCode::MALFORMED_PACKET:
						return DisconnectReasonCode::MALFORMED_PACKET;
					case DecodeErrorCode::UNSPECIFIED_ERROR:
					case DecodeErrorCode::NO_ERROR:
						break;
					}

					return DisconnectReasonCode::UNSPECIFIED_ERROR;
				}
			};

			struct EncodeResult
			{
				EncodeResult() = default;
				EncodeResult(EncodeErrorCode errorCode, std::string errorReason = "") noexcept
					: code{ errorCode }, reason{ errorReason }
				{
				}

				PacketType packetType{ PacketType::RESERVED };
				EncodeErrorCode code{ EncodeErrorCode::NO_ERROR };
				std::string reason;

				inline bool isSuccess() const noexcept
				{
					return code == EncodeErrorCode::NO_ERROR;
				}
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_ERRORCODES_H  
