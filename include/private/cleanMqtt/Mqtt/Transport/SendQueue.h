#ifndef INCLUDE_CLEANMQTT_MQTT_SENDQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_SENDQUEUE_H

#include <cleanMqtt/Mqtt/Transport/ISendJob.h>
#include <cstdint>
#include <queue>
#include <chrono>
#include <memory>
#include <string>

namespace cleanMqtt
{
	namespace mqtt
	{
#define NO_SOCKET_ERROR 0U

		struct SendBatchResult
		{
			std::size_t packetsSent{ 0U };
			std::size_t packetsAttemptedToSend{ 0U };
			std::size_t totalBytesSent{ 0U };
			int socketError{ NO_SOCKET_ERROR };
			bool isRecoverable{ true };
			std::string unrecoverableReasonStr;
			SendResultData lastSendResult;
		};

		using PacketSendJobPtr = std::unique_ptr<ISendJob>;

		class SendQueue
		{
		public:
			SendQueue() noexcept;
			virtual ~SendQueue();

			void addToQueue(PacketSendJobPtr packetSendJob);
			void sendNextBatch(SendBatchResult& outResult);
			void clearQueue() noexcept;
			bool trySendBatch(SendBatchResult& outResult, SendResultData& outLastSendResult);

		private:

			std::uint8_t m_currentLocalRetry{ 0U };
			SendResultData m_lastSendData{ 0, true, NoSendReason::NONE,{}, 0 };

			const std::uint8_t k_maxSendBatchRetries{ 3U };
			std::uint8_t m_sendBatchRetryCount{ 0U };
			const std::chrono::milliseconds k_retryDelayMs{ 1000 };
			std::chrono::steady_clock::time_point m_lastRetryTime;

			std::queue<PacketSendJobPtr> m_queuedJobs;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_SENDQUEUE_H 
