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
			INTERNAL_ERROR
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
				: packetSize{ size }, wasSent{ sent }, noSendReason{ reason }, socketError{ error }
			{
			}

			std::size_t packetSize{ 0U };
			bool wasSent{ false };
			NoSendReason noSendReason{ NoSendReason::NONE };
			int socketError{ 0 };
		};

		using PacketSendJob = std::function<interfaces::SendResultData(bool, std::size_t)>;

		class PUBLIC_API ISendQueue
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)

			ISendQueue() noexcept {};
			virtual ~ISendQueue() {};

			virtual void addToQueue(PacketSendJob packetSendJob, const mqtt::packets::PacketType type) = 0;
			virtual void sendNextBatch(SendBatchResult& outResult) = 0;
			virtual void clearQueue() noexcept = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H