#include "cleanMqtt/Mqtt/ReceiveQueue.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include "cleanMqtt/Mqtt/Packets/PacketUtils.h"
#include "cleanMqtt/Mqtt/Packets/ErrorCodes.h"

#include <vector>

namespace cleanMqtt
{
	namespace mqtt
	{
		ReceiveQueue::ReceiveQueue(interfaces::IReceiveClientPacketHandler* packetHandler) noexcept
			:m_handler{ packetHandler }
		{
			assert(m_handler != nullptr);
		}

		ReceiveQueue::~ReceiveQueue()
		{
			m_handler = nullptr;
		}

		void ReceiveQueue::addToQueue(ByteBuffer&& byteBuffer)
		{
			LockGuard lockguard{ m_mutex };

			m_queuedData.push(std::move(byteBuffer));
		}

		void ReceiveQueue::receiveAvailablePackets()
		{
			LockGuard lockguard{ m_mutex };

			packets::PacketType packetType{ packets::PacketType::RESERVED };
			m_failedDecodeResults.clear();
			packets::DecodeResult decodeResult;

			while (!m_queuedData.empty())
			{
				packetType = packets::checkPacketType(m_queuedData.front().bytes(), m_queuedData.front().size());
				decodeResult.packetType = packetType;

				switch (packetType)
				{
				case packets::PacketType::CONNECT_ACKNOWLEDGE:
				{
					packets::ConnectAck packet{ std::move(m_queuedData.front()) };
					decodeResult = std::move(packet.decode());

					if (tryAddFailedResult(std::move(decodeResult)))
					{
						break;
					}

					m_handler->handleReceivedConnectAcknowledge(packet);
					break;
				}
				case packets::PacketType::PUBLISH:
					//TODO
					break;
				case packets::PacketType::PUBLISH_ACKNOWLEDGE:
					//TODO
					break;
				case packets::PacketType::PUBLISH_COMPLETE:
					//TODO
					break;
				case packets::PacketType::PUBLISH_RECEIVED:
					//TODO
					break;
				case packets::PacketType::PUBLISH_RELEASED:
					//TODO
					break;
				case packets::PacketType::SUBSCRIBE_ACKNOWLEDGE:
					//TODO
					break;
				case packets::PacketType::UNSUBSCRIBE_ACKNOWLEDGE:
					//TODO
					break;
				case packets::PacketType::PING_RESPONSE:
					//TODO
					break;
				case packets::PacketType::DISCONNECT:
				{
					packets::Disconnect packet{ std::move(m_queuedData.front()) };
					decodeResult = std::move(packet.decode());

					if (tryAddFailedResult(std::move(decodeResult)))
					{
						break;
					}

					m_handler->handleReceivedDisconnect(packet);
					break;
				}
				}

				m_queuedData.pop();
			}
		}

		void ReceiveQueue::clearQueue()
		{
			LockGuard lockguard{ m_mutex };

			std::queue<ByteBuffer> emptyQueue;
			m_queuedData.swap(emptyQueue);
		}

		inline bool ReceiveQueue::tryAddFailedResult(packets::DecodeResult&& result) noexcept
		{
			if (result.isSuccess())
			{
				return false;
			}

			m_failedDecodeResults.push_back(std::move(result));

			return true;
		}
	}
}