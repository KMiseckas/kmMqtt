#include "cleanMqtt/Mqtt/ReceiveQueue.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include "cleanMqtt/Mqtt/Packets/PacketUtils.h"
#include "cleanMqtt/Mqtt/Packets/ErrorCodes.h"

namespace cleanMqtt
{
	namespace mqtt
	{
		ReceiveQueue::ReceiveQueue() noexcept
		{
		}

		ReceiveQueue::~ReceiveQueue()
		{
			clear();
		}

		void ReceiveQueue::addToQueue(ByteBuffer&& byteBuffer)
		{
			LockGuard guard{ m_mutex };

			m_inQueueData.push(std::move(byteBuffer));
		}

		const packets::DecodeResult ReceiveQueue::receiveNextBatch()
		{
			packets::DecodeResult decodeResult;
			decodeResult.code = packets::DecodeErrorCode::NO_ERROR;

			m_mutex.lock();
			if (m_inQueueData.empty())
			{
				m_mutex.unlock();
				return decodeResult;
			}

			std::swap(m_inQueueData, m_inProgressData);
			m_mutex.unlock();

			packets::PacketType packetType{ packets::PacketType::RESERVED };

			while (!m_inProgressData.empty())
			{
				packetType = packets::checkPacketType(m_inProgressData.front().bytes(), m_inProgressData.front().size());
				decodeResult.packetType = packetType;

				switch (packetType)
				{
				case packets::PacketType::CONNECT_ACKNOWLEDGE:
				{
					packets::ConnectAck packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess())
					{
						return decodeResult;
					}

					if (m_conAckCallback != nullptr)
					{
						m_conAckCallback(packet);
					}
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
					packets::Disconnect packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess())
					{
						return decodeResult;
					}

					if (m_DisconnectCallback != nullptr)
					{
						m_DisconnectCallback(packet);
					}
					break;
				}
				}

				m_inProgressData.pop();
			}

			return decodeResult;
		}

		void ReceiveQueue::clear() noexcept
		{
			std::queue<ByteBuffer> emptyQueueData;
			std::queue<ByteBuffer> emptyProcessData;
			m_inQueueData.swap(emptyQueueData);
			m_inProgressData.swap(emptyProcessData);

			m_conAckCallback = nullptr;
			m_DisconnectCallback = nullptr;
			m_pubCallback = nullptr;
		}

		void ReceiveQueue::setConnectAcknowledgeCallback(ConAckCallback& callback) noexcept
		{
			m_conAckCallback = callback;
		}

		void ReceiveQueue::setDisconnectCallback(DisconnectCallback& callback) noexcept
		{
			m_DisconnectCallback = callback;
		}

		void ReceiveQueue::setPublishCallback(PubCallback& callback) noexcept
		{
			m_pubCallback = callback;
		}
	}
}