#include "cleanMqtt/Mqtt/Transport/SendQueue.h"
#include <cleanMqtt/Logger/Log.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		SendQueue::SendQueue() noexcept
		{
		}

		SendQueue::~SendQueue()
		{
		}

		void SendQueue::addToQueue(PacketSendJobPtr packetSendJob)
		{
			m_queuedJobs.push(std::move(packetSendJob));
		}

		void SendQueue::sendNextBatch(SendBatchResult& outResult)
		{
			outResult.packetsSent = 0;
			outResult.totalBytesSent = 0;
			outResult.packetsAttemptedToSend = m_queuedJobs.size();
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
						LogInfo("SendQueue", "Attempting to retry failed packet. Retry turn: %d | Max Retries Allowed per Packet: %d.", m_currentLocalRetry, maxLocalRetries);

						if (m_currentLocalRetry == maxLocalRetries)
						{
							outResult.socketError = m_lastSendData.socketError;
							++m_sendBatchRetryCount;
							m_lastRetryTime = std::chrono::steady_clock::now();
						}

						continue;
					}
					else if (m_lastSendData.noSendReason == interfaces::NoSendReason::OVER_MAX_PACKET_SIZE ||
						m_lastSendData.noSendReason == interfaces::NoSendReason::ENCODE_ERROR)
					{
						m_sendBatchRetryCount = 0;
						outResult.isRecoverable = false;
						outResult.unrecoverableReasonStr = m_lastSendData.encodeResult.reason;
						outResult.lastSendResult = m_lastSendData;

						return;
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

			outResult.lastSendResult = m_lastSendData;
		}

		void SendQueue::clearQueue() noexcept
		{
			while (!m_queuedJobs.empty())
			{
				m_queuedJobs.front()->cancel();
				m_queuedJobs.pop();
			}
		}

		bool SendQueue::trySendBatch(SendBatchResult& outResult, interfaces::SendResultData& outLastSendResult)
		{
			if (m_queuedJobs.size() <= 0)
			{
				//Early successful return, no packets to proccess for sending.
				return true;
			}

			LogTrace("SendQueue", "Processing queue of %d outgoing packets.", m_queuedJobs.size());

			while (!m_queuedJobs.empty())
			{
				auto result = m_queuedJobs.front()->send();

				if (!result.wasSent)
				{
					outLastSendResult = std::move(result);
					return false;
				}

				m_queuedJobs.pop();

				outResult.packetsSent += 1;
				outResult.totalBytesSent += result.packetSize;
			}

			LogTrace("SendQueue", "Finished processing outgoing packets.");

			return true;
		}
	}
}
