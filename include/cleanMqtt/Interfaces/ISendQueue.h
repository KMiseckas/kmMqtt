#ifndef INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H
#define INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include <cleanMqtt/Interfaces/ISendJob.h>
#include <functional>
#include <string>

namespace cleanMqtt
{
	namespace interfaces
	{
#define NO_SOCKET_ERROR 0U

		struct PUBLIC_API SendBatchResult
		{
			std::size_t packetsSent{ 0U };
			std::size_t packetsAttemptedToSend{ 0U };
			std::size_t totalBytesSent{ 0U };
			int socketError{ NO_SOCKET_ERROR };
			bool isRecoverable{ true };
			std::string unrecoverableReasonStr;
		};

		using PacketSendJobPtr = std::unique_ptr<interfaces::ISendJob>;

		class PUBLIC_API ISendQueue
		{
		public:
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ISendQueue)

			ISendQueue() noexcept {};
			virtual ~ISendQueue() {};

			virtual void addToQueue(PacketSendJobPtr packetSendJob) = 0;
			virtual void sendNextBatch(SendBatchResult& outResult) = 0;
			virtual void clearQueue() noexcept = 0;
		};
	}
}
#endif //INCLUDE_CLEANMQTT_INTERFACES_ISENDQUEUE_H