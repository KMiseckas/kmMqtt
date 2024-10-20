#ifndef INCLUDE_CLEANMQTT_MQTT_DEFAULTSENDQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_DEFAULTSENDQUEUE_H

#include <cleanMqtt/Interfaces/ISendQueue.h>
#include <cstdint>
#include <queue>
#include <chrono>

namespace cleanMqtt
{
	namespace mqtt
	{
		class PUBLIC_API DefaultSendQueue : public interfaces::ISendQueue
		{
		public:
			DefaultSendQueue() noexcept;
			virtual ~DefaultSendQueue();

			void addToQueue(std::function<interfaces::SendResultData(bool, std::size_t)> packetSendJob, const mqtt::packets::PacketType type) override;
			void sendNextBatch(interfaces::SendBatchResult& outResult) override;
			void clearQueue() noexcept override;
			bool trySendBatch(interfaces::SendBatchResult& outResult, interfaces::SendResultData& outLastSendResult);

		private:

			std::uint8_t m_currentLocalRetry{ 0U };
			interfaces::SendResultData m_lastSendData{ 0, true, interfaces::NoSendReason::NONE, 0 };

			const std::uint8_t k_maxSendBatchRetries{ 3U };
			std::uint8_t m_sendBatchRetryCount{ 0U };
			const std::chrono::milliseconds k_retryDelayMs{ 1000 };
			std::chrono::steady_clock::time_point m_lastRetryTime;

			std::queue<std::function<interfaces::SendResultData(bool, std::size_t)>> m_queuedRequests;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_DEFAULTSENDQUEUE_H 
