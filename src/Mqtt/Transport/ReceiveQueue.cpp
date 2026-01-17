// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Mqtt/Transport/ReceiveQueue.h"
#include "kmMqtt/Mqtt/Packets/PacketType.h"
#include "kmMqtt/Mqtt/Packets/PacketUtils.h"
#include "kmMqtt/Mqtt/Packets/ErrorCodes.h"
#include "kmMqtt/Mqtt/ReceiveMaximumTracker.h"

namespace kmMqtt
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

			InProgressDataGuard inProgressDataGuard{ m_inProgressData }; //RAII to clear in-progress data on exit
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
					Publish packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess())
					{
						LogInfo("ReceiveQueue", "Failed to decode packet.");
						return decodeResult;
					}

					if (packet.getVariableHeader().qos != Qos::QOS_0)
					{
						if (!m_receiveMaximumTrackerPtr->decrementReceiveAllowance(packet.getVariableHeader().packetIdentifier))
						{
							LogError("ReceiveQueue", "Receive Maximum would exceeded. Receive Maximum: %u", m_receiveMaximumTrackerPtr->getMaxReceiveAllowance());

							decodeResult.code = DecodeErrorCode::RECEIVE_MAXIMUM_EXCEEDED;
							decodeResult.reason = "Receive Maximum exceeded.";
							return decodeResult;
						}
					}
					
					if (m_pubCallback != nullptr) 
					{
						m_pubCallback(std::move(packet));
					};

					break;
				}
				case PacketType::PUBLISH_ACKNOWLEDGE:
				{
					PublishAck packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess())
					{
						LogInfo("ReceiveQueue", "Failed to decode packet.");
						return decodeResult;
					}

					m_receiveMaximumTrackerPtr->incrementSendAllowance(packet.getVariableHeader().packetId);

					if (m_pubAckCallback != nullptr)
					{
						m_pubAckCallback(std::move(packet));
					};

					break;
				}
				case PacketType::PUBLISH_COMPLETE:
				{
					PublishComp packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess()) 
					{
						LogInfo("ReceiveQueue", "Failed to decode packet.");
						return decodeResult;
					}

					m_receiveMaximumTrackerPtr->incrementSendAllowance(packet.getVariableHeader().packetId);

					if (m_pubCompCallback != nullptr) 
					{
						m_pubCompCallback(std::move(packet));
					};
					break;
				}
				case PacketType::PUBLISH_RECEIVED:
				{
					PublishRec packet{ std::move(m_inProgressData.front()) };
					decodeResult = packet.decode();

					if (!decodeResult.isSuccess())
					{
						LogInfo("ReceiveQueue", "Failed to decode packet.");
						return decodeResult;
					}

					if (packet.getVariableHeader().reasonCode >= PubRecReasonCode::UNSPECIFIED_ERROR)
					{
						m_receiveMaximumTrackerPtr->incrementSendAllowance(packet.getVariableHeader().packetId);
					}

					if (m_pubRecCallback != nullptr) 
					{
						m_pubRecCallback(std::move(packet));
					};
					break;
				}
				case PacketType::PUBLISH_RELEASED:
				{
					HANDLE_RECEIVED_PACKET(PublishRel, m_pubRelCallback);
					break;
				}
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
			m_pubCompCallback = nullptr;
			m_pubRecCallback = nullptr;
			m_pubRelCallback = nullptr;
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

		void ReceiveQueue::setReceiveMaximumTracker(ReceiveMaximumTracker* const tracker) noexcept
		{
			m_receiveMaximumTrackerPtr = tracker;
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

		void ReceiveQueue::setPublishCompleteCallback(PubCompCallback& callback) noexcept
		{
			m_pubCompCallback = callback;
		}

		void ReceiveQueue::setPublishReceivedCallback(PubRecCallback& callback) noexcept
		{
			m_pubRecCallback = callback;
		}

		void ReceiveQueue::setPublishReleaseCallback(PubRelCallback& callback) noexcept
		{
			m_pubRelCallback = callback;
		}
	}
}