#ifndef INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H
#define INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include <functional>
#include <string>

namespace cleanMqtt
{
	namespace interfaces
	{
#define NO_SOCKET_ERROR 0U

		enum class NoSendReason : std::uint8_t
		{
			NONE,
			SOCKET_SEND_ERROR,
			OVER_MAX_PACKET_SIZE,
		};

		struct PUBLIC_API SendBatchResult
		{
			std::size_t packetsSent{ 0U };
			std::size_t packetsAttemptedToSend{ 0U };
			std::size_t totalBytesSent{ 0U };
			int socketError{ NO_SOCKET_ERROR };
			bool isRecoverable{ true };
			std::string unrecoverableReasonStr;
		};

		struct PUBLIC_API SendResultData
		{
			SendResultData(std::size_t size, bool sent, NoSendReason reason, int error = 0) noexcept
				: packetSize{ size }, wasSent{ sent }, noSendReason{ reason }, sendError{ error }
			{
			}

			std::size_t packetSize{ 0U };
			bool wasSent{ false };
			NoSendReason noSendReason{ NoSendReason::NONE };
			int sendError{ 0 };
		};

		class PUBLIC_API ISendQueue
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)
				DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)

			ISendQueue() noexcept {};
			virtual ~ISendQueue() {};

			virtual void addToQueue(std::function<SendResultData(bool, std::size_t)> packetSendJob, const mqtt::packets::PacketType type) = 0;
			virtual void sendNextBatch(SendBatchResult& outResult) = 0;
			virtual void clearQueue() noexcept = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H