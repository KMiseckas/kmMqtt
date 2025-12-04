#ifndef INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Mqtt/Packets/ErrorCodes.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Mqtt/Packets/Ping/PingResp.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishComp.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishRec.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishRel.h"

#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
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

			std::mutex m_mutex;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H 
