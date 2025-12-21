#ifndef INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTIMPL_H
#define INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTIMPL_H

#include "cleanMqtt/Config.h"
#include "cleanMqtt/Interfaces/IWebSocket.h"
#include "cleanMqtt/Mqtt/ClientError.h"
#include "cleanMqtt/Mqtt/Enums/ConnectionStatus.h"
#include "cleanMqtt/Mqtt/MqttClientEvents.h"
#include "cleanMqtt/Mqtt/MqttConnectionInfo.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/UnSubscribe/UnSubscribeAck.h"
#include "cleanMqtt/Mqtt/Params/ConnectArgs.h"
#include "cleanMqtt/Mqtt/Params/DisconnectArgs.h"
#include "cleanMqtt/Mqtt/Params/PubAckOptions.h"
#include "cleanMqtt/Mqtt/Params/PublishOptions.h"
#include "cleanMqtt/Mqtt/Params/SubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/Topic.h"
#include "cleanMqtt/Mqtt/Params/UnSubscribeOptions.h"
#include "cleanMqtt/Mqtt/Transport/ReceiveQueue.h"
#include "cleanMqtt/Mqtt/Transport/SendQueue.h"
#include "cleanMqtt/MqttClientOptions.h"
#include "cleanMqtt/Utils/Deferrer.h"
#include "cleanMqtt/Utils/PacketIdPool.h" 
#include <cleanMqtt/Interfaces/IMqttEnvironment.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <cleanMqtt/Mqtt/Params/PubRecOptions.h>
#include <cleanMqtt/Mqtt/Params/PubRelOptions.h>
#include <cleanMqtt/Mqtt/Params/PubCompOptions.h>

namespace cleanMqtt
{
#define DISPATCH_EVENT_TO_CONSUMER(...)\
if(m_clientOptions.isUsingInternalCallbackDeferrer())\
{\
	m_eventDeferrer.defer(__VA_ARGS__);\
}\
else\
{\
	m_clientOptions.getCallbackDispatcher()->dispatch(__VA_ARGS__);\
}\
\

	namespace mqtt
	{
		//Internal Events
		using SendPubAckEvent = events::Event<std::uint16_t>;

		class ReqResult;

		class MqttClientImpl
		{
		public:
			explicit MqttClientImpl(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions);
			~MqttClientImpl();

			ReqResult connect(ConnectArgs&& args, ConnectAddress&& address) noexcept;
			ReqResult publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept;
			ReqResult subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept;
			ReqResult unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept;
			ReqResult disconnect(DisconnectArgs&& args = {}) noexcept;
			ClientError shutdown() noexcept;

			ClientError tick() noexcept;
			void tickAsync() noexcept;

			ErrorEvent& onErrorEvent() noexcept;
			ConnectEvent& onConnectEvent() noexcept;
			DisconnectEvent& onDisconnectEvent() noexcept;
			ReconnectEvent& onReconnectEvent() noexcept;
			PublishEvent& onPublishEvent() noexcept;
			PublishCompletedEvent& onPublishCompletedEvent() noexcept;
			SubscribeAckEvent& onSubscribeAckEvent() noexcept;
			UnSubscribeAckEvent& onUnSubscribeAckEvent() noexcept;

			ConnectionStatus getConnectionStatus() const noexcept;
			const MqttConnectionInfo& getConnectionInfo() const noexcept;
			bool getIsTickingAsync() const noexcept;

		private:
			void pubAck(std::uint16_t packetId, PubAckReasonCode code, PubAckOptions&& options) noexcept;
			void pubRec(std::uint16_t packetId, PubRecReasonCode code, PubRecOptions&& options) noexcept;
			void pubRel(std::uint16_t packetId, PubRelReasonCode code, PubRelOptions&& options) noexcept;
			void pubComp(std::uint16_t packetId, PubCompReasonCode code, PubCompOptions&& options) noexcept;

			bool tryStartBrokerRedirection(std::uint8_t failedConnectionReasonCode, const Properties& properties) noexcept;
			void reconnect();

			void handleInternalDisconnect(DisconnectReasonCode reason, const DisconnectArgs& args = {}) noexcept;
			void handleExternalDisconnect(const Disconnect& packet);
			void handleExternalDisconnect(int closeCode = -1, std::string reason = "");
			void clearState() noexcept;

			void handleSocketConnectEvent(bool success);
			void handleSocketDisconnectEvent();
			void handleSocketDataReceivedEvent(ByteBuffer&& buffer);
			void handleSocketErrorEvent(int error);

			void handlePingSentEvent();
			void handlePubCompSentEvent(std::uint16_t packetId);
			void handlePubRecSentEvent(std::uint16_t packetId);
			void handlePubRelSentEvent(std::uint16_t packetId);
			void handleDisconnectSentEvent();

			void handleReceivedConnectAcknowledge(ConnectAck&& packet);
			void handleReceivedDisconnect(Disconnect&& packet);
			void handleReceivedPublish(Publish&& packet);
			void handleReceivedPublishAck(PublishAck&& packet);
			void handleReceivedPublishComp(PublishComp&& packet);
			void handleReceivedPublishRec(PublishRec&& packet);
			void handleReceivedPublishRel(PublishRel&& packet);
			void handleReceivedSubscribeAcknowledge(SubscribeAck&& packet);
			void handleReceivedUnSubscribeAcknowledge(UnSubscribeAck&& packet);
			void handleReceivedPingResponse(PingResp&& packet);

			void firePublishReceivedEvent(Publish&& packet) noexcept;

			void tickCheckTimeOut();
			void tickCheckKeepAlive();
			void tickSendPackets();
			void tickReceivePackets();
			void tickPendingPublishMessageRetries();

			void handleFailedReconnect(ConnectAck&& packet);
			void handleFailedConnect(ConnectAck&& packet);
			void handleTimeOutConnect();
			void handleTimeOutReconnect();
			void handleDecodeError(const DecodeResult& result) noexcept;

			int sendPacket(const BasePacket& packet);

			ClientError shutdownAsync() noexcept;
			ClientError shutdownCleanup() noexcept;

			std::thread m_mqttMainThread;
			std::condition_variable m_mqttMainThreadCondition;
			std::atomic<bool> m_isRunningAsync{ false };

			MqttClientOptions m_clientOptions;
			MqttConnectionInfo m_connectionInfo;
			ConnectionStatus m_connectionStatus{ ConnectionStatus::DISCONNECTED };

			events::Deferrer m_eventDeferrer;
			ErrorEvent m_errorEvent;
			ConnectEvent m_connectEvent;
			DisconnectEvent m_disconnectEvent;
			ReconnectEvent m_reconnectEvent;
			PublishEvent m_publishEvent;
			PublishCompletedEvent m_pubCompletedEvent;
			SubscribeAckEvent m_subAckEvent;
			UnSubscribeAckEvent m_unSubAckEvent;

			SendPubAckEvent m_sendPubAckEvent;

			SendQueue m_sendQueue;
			ReceiveQueue m_receiveQueue;

			SendBatchResult m_batchResultData;

			std::mutex m_mutex;
			std::mutex m_tickMutex;
			std::mutex m_receiverMutex;

			Config m_config;
			PacketIdPool m_packetIdPool;

			std::shared_ptr<IWebSocket> m_socket{ nullptr };
			ByteBuffer m_leftOverBuffer{ 0U };

			DisconnectReasonCode m_gracefulDisconnectReason{ DisconnectReasonCode::NORMAL_DISCONNECTION };
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_MQTTCLIENTIMPL_H
