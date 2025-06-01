#ifndef INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H
#define INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/ByteBuffer.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Mqtt/Packets/Ping/PingResp.h"

#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using FailedDecodeResults = std::vector<packets::DecodeResult>;

		using ConAckCallback = std::function<void(const packets::ConnectAck&)>;
		using DisconnectCallback = std::function<void(const packets::Disconnect&)>;
		using PubCallback = std::function<void(const packets::Publish&)>;
		//using PubCompCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using PubRecvCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using PubRelCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using SubAckCallback = void (MqttClient::*)(const packets::ConnectAck&);
		//using UnSubAckCallback = void (MqttClient::*)(const packets::ConnectAck&);
		using PingRespCallback = std::function<void(const packets::PingResp&)>;

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
			//void setPublishCompleteCallback() noexcept;
			//void setPublishReceivedCallback() noexcept;
			//void setPublishReleasedCallback() noexcept;
			//void setSubscribeAcknowledgeCallback() noexcept;
			//void setUnsubscribeAcknowledgeCallback() noexcept;
			void setPingResponseCallback(PingRespCallback& callback) noexcept;

		private:
			bool tryAddFailedResult(packets::DecodeResult&& result) noexcept;

			std::queue<ByteBuffer> m_inQueueData;
			std::queue<ByteBuffer> m_inProgressData;

			//Callbacks
			ConAckCallback m_conAckCallback;
			DisconnectCallback m_DisconnectCallback;
			PubCallback m_pubCallback;
			PingRespCallback m_pingRespCallback;

			std::mutex m_mutex;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_RECEIVEQUEUE_H 
