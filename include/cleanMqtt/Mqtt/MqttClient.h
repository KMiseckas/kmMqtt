#pragma once

#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Utils/Event.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/ConnectionStatus.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"
#include "cleanMqtt/Interfaces/ISendQueue.h"
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Disconnect.h"

#include <cstring>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using LockGuard = std::lock_guard<std::mutex>;

		using ConnectedEvent = events::Event<const packets::ConnectAck&>;
		using DisconnectedEvent = events::Event<const packets::DisconnectReasonCode>;

		class PUBLIC_API MqttClient
		{
			MqttClient(std::unique_ptr<interfaces::IWebSocket> socket, std::unique_ptr<interfaces::ISendQueue> sendQueue);
			~MqttClient();

			MqttClient(const MqttClient&) = delete;
			MqttClient(const MqttClient&&) = delete;

			MqttClient& operator=(MqttClient&) = delete;
			MqttClient& operator=(MqttClient&&) = delete;

			bool connect(ConnectArgs&& args, const std::string& url);
			void publish(const char* topic, const char* payloadMsg);
			void subscribe(const char* topic);
			void unSubscribe();
			void disconnect(DisconnectArgs&& args);
			void tick(float deltaTime);

			const ConnectedEvent& onConnectedEvent() const noexcept { return m_connectedEvent; }
			const DisconnectedEvent& onDisconnectedEvent() const noexcept { return m_disconnectedEvent; }

			inline ConnectionStatus getConnectionStatus() const noexcept;

		private:
			void handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args = {});
			void handleExternalDisconnect(const packets::Disconnect& packet);
			int sendPacket(const packets::BasePacket& packet);

			MqttConnectionInfo m_connectionInfo{ "CLIENT_ID" };
			ConnectionStatus m_connectionStatus{ ConnectionStatus::DISCONNECTED };

			std::unique_ptr<interfaces::IWebSocket> m_socket{ nullptr };

			ConnectedEvent m_connectedEvent;
			DisconnectedEvent m_disconnectedEvent;

			std::unique_ptr<interfaces::ISendQueue> m_sendQueue{ nullptr };
			std::queue<ByteBuffer*> m_queuedResponseData;

			interfaces::SendBatchResult m_batchResultData;

			std::mutex m_mutex;
		};
	}
}