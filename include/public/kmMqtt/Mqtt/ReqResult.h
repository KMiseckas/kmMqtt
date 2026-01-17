#ifndef INCLUDE_KMMQTT_MQTT_REQRESULT_HEADER
#define INCLUDE_KMMQTT_MQTT_REQRESULT_HEADER

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/ClientError.h>

namespace kmMqtt
{
	namespace mqtt
	{
#define NO_PACKET_ID 0U

		/**
		 * Contains the result of an MQTT client request operation.
		 * Includes any error information and the associated packet identifier if applicable.
		 */
		class PUBLIC_API ReqResult
		{
		public :

			ReqResult() noexcept = default;

			explicit ReqResult(ClientError error, std::uint16_t packetId = NO_PACKET_ID) noexcept
				: error{ std::move(error) }, packetId{ packetId } 
			{
			}

			explicit ReqResult(ClientErrorCode errorCode, const char* errorMsg, std::uint16_t packetId = NO_PACKET_ID) noexcept
				: error{ ClientError{errorCode, errorMsg} }, packetId{ packetId }
			{
			}

			/**
			 * @brief Checks if the request resulted in no error.
			 * 
			 * @return true if there was no error; false otherwise.
			 */
			inline bool noError() const noexcept { return error.noError(); }

			/**
			 * @brief Checks if a valid packet identifier is associated with the request.
			 * 
			 * @return true if a valid packet identifier exists; false otherwise.
			 */
			inline bool hasPacketId() const noexcept { return packetId != NO_PACKET_ID; }

			/**
			 * @brief Retrieves the error code associated with the request.
			 * 
			 * @return The ClientErrorCode of the request.
			 */
			inline ClientErrorCode errorCode() const noexcept { return error.errorCode; }

			/**
			 * @brief Retrieves the error message associated with the request.
			 * 
			 * @return The error message as a C-style string.
			 */
			inline const char* errorMsg() const noexcept { return error.errorMsg.c_str(); }

			ClientError error{ClientErrorCode::No_Error};

			//Associated Packet Identifier for the request, if applicable. Otherwise == NO_PACKET_ID.
			std::uint16_t packetId{ NO_PACKET_ID };
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_REQRESULT_HEADER
