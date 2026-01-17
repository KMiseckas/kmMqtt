#ifndef INCLUDE_KMMQTT_MQTT_PACKETS_ERRORCODES_H
#define INCLUDE_KMMQTT_MQTT_PACKETS_ERRORCODES_H

#include <cstdint>
#include <kmMqtt/Mqtt/Packets/Connection/Codes/DisconnectReasonCode.h>
#include <kmMqtt/Mqtt/Packets/PacketType.h>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class DecodeErrorCode : std::uint8_t
		{
			NO_ERROR = 0U,
			UNSPECIFIED_ERROR,
			MALFORMED_PACKET,
			PROTOCOL_ERROR,
			INTERNAL_ERROR,
			RECEIVE_MAXIMUM_EXCEEDED
		};

		enum class EncodeErrorCode : std::uint8_t
		{
			NO_ERROR = 0U,
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
				case DecodeErrorCode::RECEIVE_MAXIMUM_EXCEEDED:
					return DisconnectReasonCode::RECEIVE_MAXIMUM_EXCEEDED;
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
			std::uint16_t packetId{ 0U };

			inline bool isSuccess() const noexcept
			{
				return code == EncodeErrorCode::NO_ERROR;
			}
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PACKETS_ERRORCODES_H  
