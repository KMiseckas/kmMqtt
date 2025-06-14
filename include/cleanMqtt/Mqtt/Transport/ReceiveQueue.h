#ifndef INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Mqtt/Packets/Ping/PingResp.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"

#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using FailedDecodeResults = std::vector<packets::DecodeResult>;

		using ConAckCallback = std::function<void(packets::ConnectAck&&)>;
		using DisconnectCallback = std::function<void(packets::Disconnect&&)>;
		using PubCallback = std::function<void(packets::Publish&&)>;
		using PubAckCallback = std::function<void(packets::PublishAck&&)>;
		//using PubCompCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using PubRecvCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using PubRelCallback = void (MqttClient::*)(const packets::ConnectAck&);
		using SubAckCallback = std::function<void(packets::SubscribeAck&&)>;
		//using UnSubAckCallback = void (MqttClient::*)(const packets::ConnectAck&);
		using PingRespCallback = std::function<void(packets::PingResp&&)>;

		class ReceiveQueue
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(ReceiveQueue)

			ReceiveQueue() noexcept;
			~ReceiveQueue();

			void addToQueue(ByteBuffer&& byteBuffer);
			const packets::DecodeResult receiveNextBatch();
			void clear() noexcept;

			void setConnectAcknowledgeCallback(ConAckCallback& callback) noexcept;
			void setDisconnectCallback(DisconnectCallback& callback) noexcept;
			void setPublishCallback(PubCallback& callback) noexcept;
			void setPublishAcknowledgeCallback(PubAckCallback& callback) noexcept;
			//void setPublishCompleteCallback() noexcept;
			//void setPublishReceivedCallback() noexcept;
			//void setPublishReleasedCallback() noexcept;
			void setSubscribeAcknowledgeCallback(SubAckCallback& callback) noexcept;
			//void setUnsubscribeAcknowledgeCallback() noexcept;
			void setPingResponseCallback(PingRespCallback& callback) noexcept;
			//TODO: Add more callbacks as needed

		private:
			bool tryAddFailedResult(packets::DecodeResult&& result) noexcept;

			std::queue<ByteBuffer> m_inQueueData;
			std::queue<ByteBuffer> m_inProgressData;

			//Callbacks
			ConAckCallback m_conAckCallback;
			DisconnectCallback m_DisconnectCallback;
			PubCallback m_pubCallback;
			PubAckCallback m_pubAckCallback;
			PingRespCallback m_pingRespCallback;
			SubAckCallback m_subAckCallback;

			std::mutex m_mutex;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H 
