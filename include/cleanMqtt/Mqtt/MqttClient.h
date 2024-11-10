#pragma once

#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Utils/Event.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/ConnectionStatus.h>
#include <cleanMqtt/Mqtt/ReconnectionStatus.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"
#include "cleanMqtt/Mqtt/Packets/Publish/Publish.h"
#include "cleanMqtt/Interfaces/ISendQueue.h"
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include "cleanMqtt/Mqtt/ReceiveQueue.h"
#include <cleanMqtt/Utils/Deferrer.h>

#include <cstring>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using ConnectEvent = events::Event<bool, int, const packets::ConnectAck&>;
		using ReconnectEvent = events::Event<ReconnectionStatus, int, const packets::ConnectAck&>;
		using DisconnectEvent = events::Event<const packets::DisconnectReasonCode>;

		class PUBLIC_API MqttClient
		{
			MqttClient() = delete;
			MqttClient(std::unique_ptr<interfaces::IWebSocket> socket, std::unique_ptr<interfaces::ISendQueue> sendQueue);
			~MqttClient();

			MqttClient(const MqttClient&) = delete;
			MqttClient(const MqttClient&&) = delete;

			MqttClient& operator=(MqttClient&) = delete;
			MqttClient& operator=(MqttClient&&) = delete;

			void connect(ConnectArgs&& args, ConnectAddress&& address);
			void publish(const char* topic, const char* payloadMsg);
			void subscribe(const char* topic);
			void unSubscribe();
			void disconnect(DisconnectArgs&& args);
			void shutdown() noexcept;

			void tick(float deltaTime);

			const ConnectEvent& onConnectEvent() const noexcept { return m_connectEvent; }
			const DisconnectEvent& onDisconnectEvent() const noexcept { return m_disconnectEvent; }
			const ReconnectEvent& onReconnectEvent() const noexcept { return m_reconnectEvent; }

			inline ConnectionStatus getConnectionStatus() const noexcept;
			const MqttConnectionInfo& getConnectionInfo() const noexcept;

		private:
			bool tryStartBrokerRedirection(std::uint8_t failedConnectionReasonCode, const packets::Properties& properties) noexcept;
			void reconnect();

			void handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args = {});
			void handleExternalDisconnect(const packets::Disconnect& packet);
			void handleExternalDisconnect(int closeCode = -1, std::string reason = "");

			void handleSocketConnectEvent(bool success);
			void handleSocketDisconnectEvent();
			void handleSocketPacketReceivedEvent(ByteBuffer&& buffer);
			void handleSocketErrorEvent(int error);

			void handleReceivedConnectAcknowledge(const packets::ConnectAck& packet);
			void handleReceivedDisconnect(const packets::Disconnect& packet);
			void handleReceivedPublish(const packets::Publish& packet);
			//void handleReceivedPublishComplete();
			//void handleReceivedPublishReceived();
			//void handleReceivedPublishReleased();
			//void handleReceivedSubscribeAcknowledge();
			//void handleReceivedUnsubscribeAcknowledge();
			//void handleReceivedPingResponse();

			int sendPacket(const packets::BasePacket& packet);

			MqttConnectionInfo m_connectionInfo;
			ConnectionStatus m_connectionStatus{ ConnectionStatus::DISCONNECTED };

			std::unique_ptr<interfaces::IWebSocket> m_socket{ nullptr };

			events::Deferrer m_deferrer;
			ConnectEvent m_connectEvent;
			DisconnectEvent m_disconnectEvent;
			ReconnectEvent m_reconnectEvent;

			std::unique_ptr<interfaces::ISendQueue> m_sendQueue{ nullptr };
			ReceiveQueue m_receiveQueue;

			interfaces::SendBatchResult m_batchResultData;

			std::mutex m_mutex;
			std::mutex m_receiverMutex;

			//TODO create session STATE object (COnnectionInfo + SendQueue + ReceiveQueue)
		};
	}
}