#include "cleanMqtt/Mqtt/ReceiveQueue.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include "cleanMqtt/Mqtt/Packets/PacketUtils.h"
#include "cleanMqtt/Mqtt/Packets/ErrorCodes.h"

namespace cleanMqtt
{
	namespace mqtt
	{
#define HANDLE_RECEIVED_PACKET(PacketType, callback)\
packets::PacketType packet{ std::move(m_inProgressData.front()) };\
decodeResult = packet.decode();\
\
if (!decodeResult.isSuccess())\
{\
	LogInfo("ReceiveQueue", "Failed to decode packet.");\
	return decodeResult;\
}\
\
if (callback != nullptr)\
{\
	callback(packet);\
}\


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

			LogTrace("ReceiveQueue", "Processing queue of %d received packets.", m_inProgressData.size());

			while (!m_inProgressData.empty())
			{
				packetType = packets::checkPacketType(m_inProgressData.front().bytes(), m_inProgressData.front().size());
				decodeResult.packetType = packetType;

				LogInfo("ReceiveQueue", "Processing next MQTT packet.");

				if (packetType >= packets::PacketType::_COUNT)
				{
					LogTrace("ReceiveQueue", "Binary Buffer: %s", m_inProgressData.front().toString().c_str());
					LogError("ReceiveQueue", "Could not determine packet type, the packet has packet type value that's outside the range of allowed packet types. PacketType: %d",
						static_cast<std::uint8_t>(packetType));
					decodeResult.code = packets::DecodeErrorCode::PROTOCOL_ERROR;
				}

				LogInfo("ReceiveQueue", "Type: %s", mqtt::packets::k_packetTypeName[static_cast<std::uint8_t>(packetType)]);
				LogTrace("ReceiveQueue", "Binary Buffer: %s", m_inProgressData.front().toString().c_str());

				switch (packetType)
				{
				case packets::PacketType::CONNECT_ACKNOWLEDGE:
				{
					HANDLE_RECEIVED_PACKET(ConnectAck, m_conAckCallback);
					break;
				}
				case packets::PacketType::PUBLISH:
				{
					HANDLE_RECEIVED_PACKET(Publish, m_pubCallback);
					break;
				}
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
				{
					HANDLE_RECEIVED_PACKET(PingResp, m_pingRespCallback);
					break;
				}
				case packets::PacketType::DISCONNECT:
				{
					HANDLE_RECEIVED_PACKET(Disconnect, m_DisconnectCallback);
					break;
				}
				}

				m_inProgressData.pop();

				LogInfo("ReceiveQueue", "Succesfully decoded packet.");
			}

			LogTrace("ReceiveQueue", "All received packets proccessed.");
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
			m_pingRespCallback = nullptr;
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

		void ReceiveQueue::setPingResponseCallback(PingRespCallback& callback) noexcept
		{
			m_pingRespCallback = callback;
		}
	}
}