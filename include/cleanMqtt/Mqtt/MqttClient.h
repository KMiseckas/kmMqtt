#pragma once

#include <cleanMqtt/Mqtt/MqttClientEvents.h>
#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/ClientError.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/Enums/ConnectionStatus.h>
#include <cleanMqtt/Mqtt/Enums/ReconnectionStatus.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/PublishAck.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include "cleanMqtt/Mqtt/Transport/ReceiveQueue.h"
#include <cleanMqtt/Utils/Deferrer.h>
#include <cleanMqtt/Utils/PacketIdPool.h>
#include <cleanMqtt/Config.h>
#include <cleanMqtt/Mqtt/Transport/SendQueue.h>
#include <cleanMqtt/Mqtt/Params/PublishOptions.h>
#include "cleanMqtt/Mqtt/Params/PubAckOptions.h"
#include "cleanMqtt/Mqtt/Params/SubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/Topic.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h"
#include "cleanMqtt/Mqtt/Packets/Subscribe/Codes/SubAckReasonCode.h"
#include "cleanMqtt/Mqtt/State/SubAckTopicReason.h"


#include <cstring>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		//Internal Events
		using SendPubAckEvent = events::Event<PacketID>;

		class PUBLIC_API MqttClient
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient);
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient);

			MqttClient() = delete;
			MqttClient(const Config config, std::unique_ptr<interfaces::IWebSocket> socket);
			~MqttClient();

			ClientError connect(ConnectArgs&& args, ConnectAddress&& address) noexcept;
			ClientError publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept;
			ClientError subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept;
			ClientError unSubscribe(const char* topic) noexcept;
			ClientError disconnect(DisconnectArgs&& args = {}) noexcept;
			ClientError shutdown() noexcept;

			void tick(float deltaTime) noexcept;

			ErrorEvent& onErrorEvent() noexcept { return m_errorEvent; }
			ConnectEvent& onConnectEvent() noexcept { return m_connectEvent; }
			DisconnectEvent& onDisconnectEvent() noexcept { return m_disconnectEvent; }
			ReconnectEvent& onReconnectEvent() noexcept { return m_reconnectEvent; }
			PublishEvent& onPublishEvent() noexcept { return m_publishEvent; }
			PublishAckEvent& onPublishAckEvent() noexcept { return m_pubAckEvent; }
			SubscribeAckEvent& onSubscribeAckEvent() noexcept { return m_subAckEvent; }

			ConnectionStatus getConnectionStatus() const noexcept;
			const MqttConnectionInfo& getConnectionInfo() const noexcept;

		private:
			void pubAck(PacketID packetId, packets::PubAckReasonCode code, PubAckOptions&& options) noexcept;

			bool tryStartBrokerRedirection(std::uint8_t failedConnectionReasonCode, const packets::Properties& properties) noexcept;
			void reconnect();

			void handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args = {}) noexcept;
			void handleExternalDisconnect(const packets::Disconnect& packet);
			void handleExternalDisconnect(int closeCode = -1, std::string reason = "");
			void clearState() noexcept;

			void handleSocketConnectEvent(bool success);
			void handleSocketDisconnectEvent();
			void handleSocketPacketReceivedEvent(ByteBuffer&& buffer);
			void handleSocketErrorEvent(int error);

			void handleReceivedConnectAcknowledge(packets::ConnectAck&& packet);
			void handleReceivedDisconnect(packets::Disconnect&& packet);
			void handleReceivedPublish(packets::Publish&& packet);
			void handleReceivedPublishAck(packets::PublishAck&& packet);
			//void handleReceivedPublishComplete();
			//void handleReceivedPublishReceived();
			//void handleReceivedPublishReleased();
			void handleReceivedSubscribeAcknowledge(packets::SubscribeAck&& packet);
			//void handleReceivedUnsubscribeAcknowledge();
			void handleReceivedPingResponse(packets::PingResp&& packet);

			void firePublishReceivedEvent(packets::Publish&& packet) noexcept;

			void tickCheckTimeOut();
			void tickCheckKeepAlive();
			void tickSendPackets();
			void tickReceivePackets();
			void tickPendingPublishMessageRetries();

			void handleFailedReconnect(packets::ConnectAck&& packet);
			void handleFailedConnect(packets::ConnectAck&& packet);
			void handleTimeOutConnect();
			void handleTimeOutReconnect();
			void handleDecodeError(const DecodeResult& result) noexcept;

			int sendPacket(const packets::BasePacket& packet);

			MqttConnectionInfo m_connectionInfo;
			ConnectionStatus m_connectionStatus{ ConnectionStatus::DISCONNECTED };

			std::unique_ptr<interfaces::IWebSocket> m_socket{ nullptr };

			events::Deferrer m_eventDeferrer;
			ErrorEvent m_errorEvent;
			ConnectEvent m_connectEvent;
			DisconnectEvent m_disconnectEvent;
			ReconnectEvent m_reconnectEvent;
			PublishEvent m_publishEvent;
			PublishAckEvent m_pubAckEvent;
			SubscribeAckEvent m_subAckEvent;

			SendPubAckEvent m_sendPubAckEvent;

			SendQueue m_sendQueue;
			ReceiveQueue m_receiveQueue;

			SendBatchResult m_batchResultData;

			std::mutex m_mutex;
			std::mutex m_receiverMutex;

			Config m_config;
			PacketIdPool m_packetIdPool;

			ByteBuffer m_leftOverBuffer{ 0U };
		};
	}
}