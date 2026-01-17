#ifndef INCLUDE_KMMQTT_MQTT_RECEIVEQUEUE_H
#define INCLUDE_KMMQTT_MQTT_RECEIVEQUEUE_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/ByteBuffer.h"
#include "kmMqtt/Mqtt/Packets/ErrorCodes.h"
#include "kmMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "kmMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "kmMqtt/Mqtt/Packets/Publish/Publish.h"
#include "kmMqtt/Mqtt/Packets/Ping/PingResp.h"
#include "kmMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "kmMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "kmMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h"
#include "kmMqtt/Mqtt/Packets/Publish/PublishComp.h"
#include "kmMqtt/Mqtt/Packets/Publish/PublishRec.h"
#include "kmMqtt/Mqtt/Packets/Publish/PublishRel.h"

#include <queue>
#include <mutex>
#include <functional>

namespace kmMqtt
{
	namespace mqtt
	{
		using FailedDecodeResults = std::vector<DecodeResult>;

		using ConAckCallback = std::function<void(ConnectAck&&)>;
		using DisconnectCallback = std::function<void(Disconnect&&)>;
		using PubCallback = std::function<void(Publish&&)>;
		using PubAckCallback = std::function<void(PublishAck&&)>;
		using PubCompCallback = std::function<void(PublishComp&&)>;
		using PubRecCallback = std::function<void(PublishRec&&)>;
		using PubRelCallback = std::function<void(PublishRel&&)>;
		using SubAckCallback = std::function<void(SubscribeAck&&)>;
		using UnSubAckCallback = std::function<void(UnSubscribeAck&&)>;
		using PingRespCallback = std::function<void(PingResp&&)>;

		struct ReceiveMaximumTracker;

		/**
		 * @brief Queue for receiving MQTT packets into the internal system.
		 * Handles logic for processing the queue of packets received from the network, i.e. decoding.
		 * This class only takes packets as seperate buffers that have already been seperated by length from the initial network byte buffer
		 * received through socket.
		 * 
		 * Does not listen to socket for packets directly, relies on `addToQueue` calls to queue up packets received through socket.
		 */
		class ReceiveQueue
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)

			ReceiveQueue() noexcept;
			~ReceiveQueue();

			void addToQueue(ByteBuffer&& byteBuffer);
			const DecodeResult receiveNextBatch();
			void clear() noexcept;

			void setConnectAcknowledgeCallback(ConAckCallback& callback) noexcept;
			void setDisconnectCallback(DisconnectCallback& callback) noexcept;
			void setPublishCallback(PubCallback& callback) noexcept;
			void setPublishAcknowledgeCallback(PubAckCallback& callback) noexcept;
			void setPublishReceivedCallback(PubRecCallback& callback) noexcept;
			void setPublishReleaseCallback(PubRelCallback& callback) noexcept;
			void setPublishCompleteCallback(PubCompCallback& callback) noexcept;
			void setSubscribeAcknowledgeCallback(SubAckCallback& callback) noexcept;
			void setUnSubscribeAcknowledgeCallback(UnSubAckCallback& callback) noexcept;
			void setPingResponseCallback(PingRespCallback& callback) noexcept;

			void setReceiveMaximumTracker(ReceiveMaximumTracker* const tracker) noexcept;
		private:
			std::queue<ByteBuffer> m_inQueueData;
			std::queue<ByteBuffer> m_inProgressData;

			//Callbacks
			ConAckCallback m_conAckCallback;
			DisconnectCallback m_DisconnectCallback;
			PubCallback m_pubCallback;
			PubAckCallback m_pubAckCallback;
			PingRespCallback m_pingRespCallback;
			SubAckCallback m_subAckCallback;
			UnSubAckCallback m_unSubAckCallback;
			PubCompCallback m_pubCompCallback;
			PubRecCallback m_pubRecCallback;
			PubRelCallback m_pubRelCallback;

			ReceiveMaximumTracker* m_receiveMaximumTrackerPtr{ nullptr };

			std::mutex m_mutex;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_RECEIVEQUEUE_H 
