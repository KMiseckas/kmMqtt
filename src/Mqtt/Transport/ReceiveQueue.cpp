#include "cleanMqtt/Mqtt/Transport/ReceiveQueue.h"
#include "cleanMqtt/Mqtt/Packets/PacketType.h"
#include "cleanMqtt/Mqtt/Packets/PacketUtils.h"
#include "cleanMqtt/Mqtt/Packets/ErrorCodes.h"

namespace cleanMqtt
{
	namespace mqtt
	{
#define HANDLE_RECEIVED_PACKET(PacketType, callback)\
PacketType packet{ std::move(m_inProgressData.front()) };\
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
	callback(std::move(packet));\
}\

		struct InProgressDataGuard
		{
			InProgressDataGuard(std::queue<ByteBuffer>& inProgressData) noexcept
				: container{ inProgressData }
			{
			}

			~InProgressDataGuard() noexcept
			{
				std::queue<ByteBuffer> emptyContainer;
				container.swap(emptyContainer);
			}

			std::queue<ByteBuffer>& container;
		};

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

		const DecodeResult ReceiveQueue::receiveNextBatch()
		{
			DecodeResult decodeResult;
			decodeResult.code = DecodeErrorCode::NO_ERROR;

			{
				LockGuard guard{ m_mutex };
				if (m_inQueueData.empty())
				{
					return decodeResult;
				}

				std::swap(m_inQueueData, m_inProgressData);
			}

			InProgressDataGuard inProgressDataGuard{ m_inProgressData };
			PacketType packetType{ PacketType::RESERVED };

			LogTrace("ReceiveQueue", "Processing queue of %d received packets.", m_inProgressData.size());

			while (!m_inProgressData.empty())
			{
				packetType = checkPacketType(m_inProgressData.front().bytes(), m_inProgressData.front().size());
				decodeResult.packetType = packetType;

				LogInfo("ReceiveQueue", "Processing next MQTT packet.");

				if (packetType >= PacketType::_COUNT)
				{
					LogTrace("ReceiveQueue", "Binary Buffer: %s", m_inProgressData.front().toString().c_str());
					LogError("ReceiveQueue", "Could not determine packet type, the packet has packet type value that's outside the range of allowed packet types. PacketType: %d",
						static_cast<std::uint8_t>(packetType));
					decodeResult.code = DecodeErrorCode::PROTOCOL_ERROR;
				}

				LogInfo("ReceiveQueue", "Type: %s", mqtt::k_packetTypeName[static_cast<std::uint8_t>(packetType)]);
				LogTrace("ReceiveQueue", "Binary Buffer: %s", m_inProgressData.front().toString().c_str());

				switch (packetType)
				{
				case PacketType::CONNECT_ACKNOWLEDGE:
				{
					HANDLE_RECEIVED_PACKET(ConnectAck, m_conAckCallback);
					break;
				}
				case PacketType::PUBLISH:
				{
					HANDLE_RECEIVED_PACKET(Publish, m_pubCallback);
					break;
				}
				case PacketType::PUBLISH_ACKNOWLEDGE:
				{
					HANDLE_RECEIVED_PACKET(PublishAck, m_pubAckCallback);
					break;
				}
				case PacketType::PUBLISH_COMPLETE:
					//TODO
					break;
				case PacketType::PUBLISH_RECEIVED:
					//TODO
					break;
				case PacketType::PUBLISH_RELEASED:
					//TODO
					break;
				case PacketType::SUBSCRIBE_ACKNOWLEDGE:
				{
					HANDLE_RECEIVED_PACKET(SubscribeAck, m_subAckCallback);
					break;
				}
				case PacketType::UNSUBSCRIBE_ACKNOWLEDGE:
				{
					HANDLE_RECEIVED_PACKET(UnSubscribeAck, m_unSubAckCallback);
					break;
				}
				case PacketType::PING_RESPONSE:
				{
					HANDLE_RECEIVED_PACKET(PingResp, m_pingRespCallback);
					break;
				}
				case PacketType::DISCONNECT:
				{
					HANDLE_RECEIVED_PACKET(Disconnect, m_DisconnectCallback);
					break;
				}
				//Added to silence -Wswitch warning on some compilers
				case PacketType::RESERVED:
				case PacketType::CONNECT:
				case PacketType::SUBSCRIBE:
				case PacketType::UNSUBSCRIBE:
				case PacketType::PING_REQUQEST:
				case PacketType::AUTH:
				case PacketType::_COUNT:
					default:
					LogError("ReceiveQueue", "Received unsupported packet type: %s", mqtt::k_packetTypeName[static_cast<std::uint8_t>(packetType)]);
					decodeResult.code = DecodeErrorCode::PROTOCOL_ERROR;
					return decodeResult;
				}

				if (!m_inProgressData.empty())
				{
					m_inProgressData.pop();
				}

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
			m_pubAckCallback = nullptr;
			m_subAckCallback = nullptr;
			m_unSubAckCallback = nullptr;
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

		void ReceiveQueue::setPublishAcknowledgeCallback(PubAckCallback& callback) noexcept
		{
			m_pubAckCallback = callback;
		}

		void ReceiveQueue::setSubscribeAcknowledgeCallback(SubAckCallback& callback) noexcept
		{
			m_subAckCallback = callback;
		}

		void ReceiveQueue::setUnSubscribeAcknowledgeCallback(UnSubAckCallback& callback) noexcept
		{
			m_unSubAckCallback = callback;
		}
	}
}