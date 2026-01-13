#include "cleanMqtt/Mqtt/Transport/SendQueue.h"
#include "cleanMqtt/Logger/Log.h"
#include "cleanMqtt/Mqtt/ReceiveMaximumTracker.h"

namespace cleanMqtt
{
	namespace mqtt
	{
#define MAX_ALLOWED_PERSISTENT_SEND_BUFFER_SIZE (1024 * 1024) //1 MB

		SendQueue::SendQueue() noexcept
		{
		}

		SendQueue::~SendQueue()
		{
		}

		void SendQueue::setSocket(std::shared_ptr<IWebSocket> socket) noexcept
		{
			m_socket = socket;
		}

		void SendQueue::setReceiveMaximumTracker(ReceiveMaximumTracker* tracker) noexcept
		{
			m_receiveMaximumTrackerPtr = tracker;
		}

		void SendQueue::addToQueue(PacketSendJobPtr packetSendJob)
		{
			LockGuard guard{ m_mutex };
			m_nextPacketComposersBatch.push_back(std::move(packetSendJob));
		}

		void SendQueue::sendNextBatch(SendBatchResult& outResult)
		{
			outResult.totalBytesSent = 0;
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

			LockGuard guard{ m_mutex };
			while (m_currentLocalRetry < maxLocalRetries)
			{
				++m_currentLocalRetry;

				if (!trySendBatch(outResult, m_lastSendData))
				{
					if (m_startGracefulClear)
					{
						m_startGracefulClear = false;
						return;
					}

					LogInfo("SendQueue", "Failed to proccess and send an outgoing packet. Reason: %d.", static_cast<std::uint8_t>(m_lastSendData.noSendReason));

					if (m_lastSendData.noSendReason == NoSendReason::SOCKET_SEND_ERROR)
					{
						//Can retry same packets a few times before registering as a concrete fail for the overall send queue.
						LogInfo("SendQueue", "Attempting to retry failed packets. Retry turn: %d | Max Retries Allowed per Packet: %d.", m_currentLocalRetry, maxLocalRetries);

						if (m_currentLocalRetry == maxLocalRetries)
						{
							outResult.socketError = m_lastSendData.socketError;
							++m_sendBatchRetryCount;
							m_lastRetryTime = std::chrono::steady_clock::now();
						}

						continue;
					}
					else if (m_lastSendData.noSendReason == NoSendReason::OVER_MAX_PACKET_SIZE ||
						m_lastSendData.noSendReason == NoSendReason::ENCODE_ERROR)
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

				static constexpr const char* rsnStr{ "Reached max consecutive failed retries." };

				outResult.isRecoverable = false;
				outResult.unrecoverableReasonStr = rsnStr;
			}

			outResult.lastSendResult = m_lastSendData;
		}

        void SendQueue::clearQueue(const bool graceful) noexcept
        {
			if (graceful)
			{
				//Wait for next send batch to process and clear the queue.
				m_startGracefulClear = true;
			}
			else
			{
				LockGuard guard{ m_mutex };

				for (const auto& c : m_nextPacketComposersBatch)
				{
					c->cancel();
				}

				m_nextPacketComposersBatch.clear();
			}
        }

		void SendQueue::setOnPingSentCallback(const std::function<void()>& callback) noexcept
		{
			m_onPingSentCallback = callback;
		}

		void SendQueue::setOnPubCompSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept
		{
			m_onPubCompSentCallback = callback;
		}

		void SendQueue::setOnPubRelSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept
		{
			m_onPubRelSentCallback = callback;
		}

		void SendQueue::setOnPubRecSentCallback(const std::function<void(std::uint16_t)>& callback) noexcept
		{
			m_onPubRecSentCallback = callback;
		}

		void SendQueue::setOnDisconnectSentCallback(const std::function<void()>& callback) noexcept
		{
			m_onDisconnectSentCallback = callback;
		}

		bool SendQueue::trySendBatch(SendBatchResult& outResult, SendResultData& outLastSendResult)
		{
			if (m_nextPacketComposersBatch.size() <= 0)
			{
				//Early successful return, no packets to proccess for sending.
				return true;
			}

			//Check if we are in the process of gracefully clearing the send queue.
			// If so, cancel all pending packet composers and exit without sending anything.
			if (m_startGracefulClear)
			{
				LogInfo("SendQueue", "Gracefully clearing send queue.");
				for (const auto& c : m_nextPacketComposersBatch)
				{
					c->cancel();
				}
				m_nextPacketComposersBatch.clear();
				return false;
			}

			LogTrace("SendQueue", "Processing queue of %d outgoing packets.", m_nextPacketComposersBatch.size());

			std::vector<ByteBuffer> encodedDataQueue; //Encoded data ready to send through socket.
			std::size_t fullOutgoingDataSize{ m_sendBuffer.size() }; //Data size to send, init with any left over data in send buffer.
			bool hasPingPacket{ false }; //Track if there is a ping packet in the batch.
			std::size_t pingPacketLastByte{ 0 }; //Byte index in buffer where ping packet ends.

			std::vector<PacketSendJobPtr> delayedPackets; //Packets delayed due to no being allowed to send yet.

			/**
			 * First step: Compose all packets in batch into encoded data.
			 * Perform some prelimenery checks:
			 * - if packet cannot be sent now, delay it for next batch.
			 * - if packet fails to encode, return error.
			 * - track some packets for callback to listeners later.
			 * - accumulate total size of encoded data to send.
			 * - move encoded data into send queue.
			 */
			for (auto& c : m_nextPacketComposersBatch)
			{
				//Check if packet can be sent now. If not, delay it for next batch.
				if (!c->canSend())
				{
					delayedPackets.push_back(std::move(c));
					continue;
				}

				//Compose packet into encoded data.
				auto result{ c->compose() };

				//Check for encode errors.
				if (!result.encodeResult.isSuccess())
				{
					outLastSendResult = {};
					outLastSendResult.encodeResult = result.encodeResult;
					outLastSendResult.noSendReason = NoSendReason::ENCODE_ERROR;
					outLastSendResult.wasSent = false;

					outResult.isRecoverable = false;
					outResult.unrecoverableReasonStr = result.encodeResult.reason;

					return false;
				}

				//Track if there is a ping packet in the batch for callback later.
				if(result.encodeResult.packetType == PacketType::PING_REQUQEST)
				{
					if (!hasPingPacket)
					{
						//Track byte index in buffer where ping packet ends so we can notify listener when it is fully sent through socket.
						pingPacketLastByte = fullOutgoingDataSize + result.encodedData.size();
						hasPingPacket = true;
					}
				}
				else if (result.encodeResult.packetType == PacketType::PUBLISH_ACKNOWLEDGE)
				{
					assert(m_receiveMaximumTrackerPtr != nullptr);

					m_receiveMaximumTrackerPtr->incrementReceiveAllowance(result.encodeResult.packetId);
				}
				else if (result.encodeResult.packetType == PacketType::PUBLISH_COMPLETE ||
					result.encodeResult.packetType == PacketType::PUBLISH_RECEIVED ||
					result.encodeResult.packetType == PacketType::PUBLISH_RELEASED ||
					result.encodeResult.packetType == PacketType::DISCONNECT)
				{
					if (result.encodeResult.packetType == PacketType::PUBLISH_COMPLETE)
					{
						assert(m_receiveMaximumTrackerPtr != nullptr);

						m_receiveMaximumTrackerPtr->incrementReceiveAllowance(result.encodeResult.packetId);
					}

					//Track some packets by adding to buffer so we can notify listeners when they are sent successfully.
					m_packetsMetadataInBuffer.push_back({ fullOutgoingDataSize + result.encodedData.size(), result.encodeResult.packetType,  result.encodeResult.packetId });
				}
				else if (result.encodeResult.packetType == PacketType::PUBLISH)
				{
					if (c->getQos() != Qos::QOS_0)
					{
						assert(m_receiveMaximumTrackerPtr != nullptr);

						m_receiveMaximumTrackerPtr->decrementSendAllowance(result.encodeResult.packetId);
					}
				}

				fullOutgoingDataSize += result.encodedData.size();
				encodedDataQueue.push_back(std::move(result.encodedData)); //Move encoded data to send queue.
			}

			//Move delayed packets back to main batch for next send attempt.
			m_nextPacketComposersBatch = std::move(delayedPackets);

			//Ensure send buffer has enough capacity to hold all data.
			if (fullOutgoingDataSize > m_sendBuffer.capacity())
			{
				auto tempBuffer = ByteBuffer{ fullOutgoingDataSize };
				tempBuffer.append(m_sendBuffer);
				m_sendBuffer = std::move(tempBuffer);
			}

			//Append all encoded data to send buffer for sending all at once rather than packet by packet.
			for(const auto& data : encodedDataQueue)
			{
				m_sendBuffer.append(data);
			}

			static constexpr std::uint8_t kMax_Partial_Send_Loops{ 3 };
			std::uint8_t partialSendLoopCount{ 0 };

			int sendResult{ 0 };

			/**
			 * Second step: Try sending all data in send buffer.
			 * - Loop to handle partial sends up to a max number of loops.
			 * - On successful send of all data, clear buffer and return.
			 * - On partial send, remove sent data from buffer and loop to try send remaining data.
			 * - On socket send error, return error.
			 * - Notify relevant listeners of sent packets.
			 * - If max partial send loops reached, return success with bytes sent. Remained will try send next batch/tick.
			 */
			while (partialSendLoopCount <= kMax_Partial_Send_Loops)
			{
				partialSendLoopCount++;

				//Try sending data in buffer through socket.
				sendResult = sendData(m_sendBuffer);

				//If send was successful (full or partial), process result.
				if (sendResult >= 0)
				{
					outResult.controlPacketSent = true;
					outResult.totalBytesSent = sendResult;

					/**
					 * If all data was sent successfully, clear buffer and return.
					 * If only partial data was sent, remove sent data from buffer and loop to try send remaining data.
					 * Notify relevant listeners of sent packets.
					 */
					if (sendResult == static_cast<int>(m_sendBuffer.size()))
					{
						if (hasPingPacket)
						{
							//Ping packet was sent successfully.
							m_onPingSentCallback();
						}

						//Since all data was sent, notify all tracked packets as sent.
						for each(auto& metadata in m_packetsMetadataInBuffer)
						{
							switch (metadata.packetType)
							{
							case PacketType::PUBLISH_COMPLETE:
								m_onPubCompSentCallback(metadata.packetId);
								break;
							case PacketType::PUBLISH_RELEASED:
								m_onPubRelSentCallback(metadata.packetId);
								break;
							case PacketType::PUBLISH_RECEIVED:
								m_onPubRecSentCallback(metadata.packetId);
								break;
							case PacketType::PUBLISH:
								break;
							case PacketType::DISCONNECT:
								m_onDisconnectSentCallback();
								break;
							}
						}

						m_packetsMetadataInBuffer.clear();

						//Reset buffer to free up memory if over max allowed persistent size so we don't hold onto too much memory unnecessarily.
						if (m_sendBuffer.capacity() > MAX_ALLOWED_PERSISTENT_SEND_BUFFER_SIZE)
						{
							m_sendBuffer = ByteBuffer{};
						}
						else
						{
							m_sendBuffer.removeFromBeginning(sendResult);
						}

						return true;
					}
					else
					{
						if (hasPingPacket)
						{
							if (sendResult >= static_cast<int>(pingPacketLastByte))
							{
								//Ping packet was sent successfully.
								m_onPingSentCallback();
							}

							//Notify other tracked packets as sent if their end byte index is within the sent data range.
							for each(auto& metadata in m_packetsMetadataInBuffer)
							{
								if (sendResult >= static_cast<int>(metadata.endByteInBuffer))
								{
									switch (metadata.packetType)
									{
									case PacketType::PUBLISH_COMPLETE:
										m_onPubCompSentCallback(metadata.packetId);
										break;
									case PacketType::PUBLISH_RELEASED:
										m_onPubRelSentCallback(metadata.packetId);
										break;
									case PacketType::PUBLISH_RECEIVED:
										m_onPubRecSentCallback(metadata.packetId);
										break;
									case PacketType::PUBLISH:
										break;
									case PacketType::DISCONNECT:
										m_onDisconnectSentCallback();
										break;
									}

									m_packetsMetadataInBuffer.erase(m_packetsMetadataInBuffer.begin());
								}
							}
						}

						m_sendBuffer.removeFromBeginning(sendResult);
					}
				}
			}

			if (sendResult >= 0)
			{
				return true;
			}

			outLastSendResult = {};
			outLastSendResult.noSendReason = NoSendReason::SOCKET_SEND_ERROR;
			outLastSendResult.socketError = sendResult;
			outLastSendResult.wasSent = false;
			return false;
		}

		int SendQueue::sendData(const ByteBuffer& data)
		{
			if (m_socket == nullptr)
			{
				LogError("SendQueue", "Cannot send data, socket is nullptr.");
				return -1;
			}

			LogTrace("SendQueue", "Sending data. Size: %d", data.size());

			if (data.size() > 0)
			{
				int sendResult{ m_socket->send(data) };

				if (sendResult >= 0)
				{
					LogTrace("SendQueue", "Data sent, Bytes: %d of %d.", sendResult, data.size());
					return sendResult;
				}

				LogError("SendQueue", "Sending packet failed at socket level: %d", m_socket->getLastError());

				return m_socket->getLastError();
			}

			LogWarning("SendQueue", "Cannot send data, data buffer size is 0.");
			return 0;
		}
	}
}
