#pragma once

#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Utils/Event.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/ConnectionStatus.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include "cleanMqtt/Mqtt/Packets/Connection/Connect.h"
#include "cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h"

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

		using ConnectedEvent = events::Event<const packets::Connect&>;
		using DisconnectedEvent = events::Event<const packets::ConnectAck&>;

		class PUBLIC_API MqttClient
		{
			MqttClient(std::unique_ptr<interfaces::IWebSocket> socket);
			~MqttClient();

			MqttClient(const MqttClient&) = delete;
			MqttClient(const MqttClient&&) = delete;

			MqttClient& operator=(MqttClient&) = delete;
			MqttClient& operator=(MqttClient&&) = delete;

			void connect(ConnectArgs&& args);
			void publish(const char* topic, const char* payloadMsg);
			void subscribe(const char* topic);
			void unSubscribe();
			void disconnect();
			void tick(float deltaTime);

			const ConnectedEvent& onConnectedEvent() const noexcept { return m_connectedEvent; }
			const DisconnectedEvent& onDisconnectedEvent() const noexcept { return m_disconnectedEvent; }

		private:
			int sendPacket(const ByteBuffer& data);

			MqttConnectionInfo m_connectionInfo{ "CLIENT_ID" };
			ConnectionStatus m_connectionStatus{ ConnectionStatus::DISCONNECTED };

			std::unique_ptr<interfaces::IWebSocket> m_socket{ nullptr };

			ConnectedEvent m_connectedEvent;
			DisconnectedEvent m_disconnectedEvent;

			std::queue<std::function<void()>> m_queuedRequests;
			std::queue<ByteBuffer*> m_queuedResponseData;

			std::mutex m_mutex;
		};
	}
}