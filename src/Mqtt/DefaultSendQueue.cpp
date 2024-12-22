#include "cleanMqtt/Mqtt/DefaultSendQueue.h"
#include <cleanMqtt/Logger/Log.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		DefaultSendQueue::DefaultSendQueue() noexcept
		{
		}

		DefaultSendQueue::~DefaultSendQueue()
		{
		}

		void DefaultSendQueue::addToQueue(std::function<interfaces::SendResultData(bool, std::size_t)> packetSendJob, const mqtt::packets::PacketType /*type*/)
		{
			m_queuedRequests.push(std::move(packetSendJob));
		}

		void DefaultSendQueue::sendNextBatch(interfaces::SendBatchResult& outResult)
		{
			outResult.packetsSent = 0;
			outResult.totalBytesSent = 0;
			outResult.packetsAttemptedToSend = m_queuedRequests.size();
			outResult.isRecoverable = true;
			outResult.socketError = NO_SOCKET_ERROR;

			if (m_sendBatchRetryCount != 0)
			{
				if ((std::chrono::steady_clock::now() - m_lastRetryTime) < k_retryDelayMs)
				{
					return;
				}
			}

			static constexpr std::uint8_t maxLocalRetries = 2;

			m_currentLocalRetry = 0;

			while (m_currentLocalRetry < maxLocalRetries)
			{
				++m_currentLocalRetry;

				if (!trySendBatch(outResult, m_lastSendData))
				{
					LogInfo("SendQueue", "Failed to proccess and send an outgoing packet. Reason: %d.", static_cast<std::uint8_t>(m_lastSendData.noSendReason));

					if (m_lastSendData.noSendReason == interfaces::NoSendReason::SOCKET_SEND_ERROR)
					{
						//Can retry same packet a few times before registering as a concrete fail for the overall send queue.
						if (m_currentLocalRetry == maxLocalRetries)
						{
							LogInfo("SendQueue", "Attempting to retry failed packet. Retry turn: %d | Max Retries Allowed per Packet: %d.", m_currentLocalRetry, maxLocalRetries);

							outResult.socketError = m_lastSendData.sendError;
							++m_sendBatchRetryCount;
							m_lastRetryTime = std::chrono::steady_clock::now();
						}

						continue;
					}
					else if (m_lastSendData.noSendReason == interfaces::NoSendReason::OVER_MAX_PACKET_SIZE)
					{
						m_sendBatchRetryCount = 0;
						break;
					}

					break;
				}

				m_sendBatchRetryCount = 0;
				break;
			}

			if (m_sendBatchRetryCount >= k_maxSendBatchRetries)
			{
				LogInfo("SendQueue", "Failed send queue processing.");

				static constexpr char* rsnStr{ "Reached max consecutive failed retries." };

				outResult.isRecoverable = false;
				outResult.unrecoverableReasonStr = rsnStr;
			}
		}

		void DefaultSendQueue::clearQueue() noexcept
		{
			std::queue< std::function<interfaces::SendResultData(bool, std::size_t)>> empty;
			m_queuedRequests.swap(empty);
		}

		bool DefaultSendQueue::trySendBatch(interfaces::SendBatchResult& outResult, interfaces::SendResultData& outLastSendResult)
		{
			if (m_queuedRequests.size() <= 0)
			{
				//Early successful return, no packets to proccess for sending.
				return true;
			}

			LogTrace("SendQueue", "Processing queue of %d outgoing packets.", m_queuedRequests.size());

			while (!m_queuedRequests.empty())
			{
				auto result = m_queuedRequests.front()(false, 0);

				if (!result.wasSent)
				{
					outLastSendResult = std::move(result);
					return false;
				}

				m_queuedRequests.pop();

				outResult.packetsSent += 1;
				outResult.totalBytesSent += result.packetSize;
			}

			LogTrace("SendQueue", "Finished processing outgoing packets.");

			return true;
		}
	}
}
