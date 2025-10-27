#pragma once

#include "cleanMqtt/MqttClientOptions.h"
#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Config.h"
#include <cleanMqtt/Interfaces/IMqttEnvironment.h>
#include "cleanMqtt/Interfaces/IWebSocket.h"
#include <cleanMqtt/Environments/DefaultEnvironmentFactory.h>
#include <cleanMqtt/Mqtt/ClientError.h>
#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include "cleanMqtt/Mqtt/MqttClientEvents.h"
#include "cleanMqtt/Mqtt/Enums/ConnectionStatus.h"
#include <cleanMqtt/Mqtt/Enums/ReconnectionStatus.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include <cleanMqtt/Mqtt/Params/PublishOptions.h>
#include "cleanMqtt/Mqtt/Params/PubAckOptions.h"
#include "cleanMqtt/Mqtt/Params/SubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/UnSubscribeOptions.h"
#include "cleanMqtt/Mqtt/Params/Topic.h"

#include <memory>

namespace cleanMqtt
{
	namespace mqtt
	{
		class MqttClientImpl;

		class PUBLIC_API MqttClient
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient);
			DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(MqttClient);

			MqttClient();
			MqttClient(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions);
			~MqttClient();

			ClientError connect(ConnectArgs&& args, ConnectAddress&& address) noexcept;
			ClientError publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept;
			ClientError subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept;
			ClientError unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept;
			ClientError disconnect(DisconnectArgs&& args = {}) noexcept;
			ClientError shutdown() noexcept;

			ClientError tick() noexcept;

			ErrorEvent& onErrorEvent() noexcept;
			ConnectEvent& onConnectEvent() noexcept;
			DisconnectEvent& onDisconnectEvent() noexcept;
			ReconnectEvent& onReconnectEvent() noexcept;
			PublishEvent& onPublishEvent() noexcept;
			PublishAckEvent& onPublishAckEvent() noexcept;
			SubscribeAckEvent& onSubscribeAckEvent() noexcept;

			ConnectionStatus getConnectionStatus() const noexcept;
			const MqttConnectionInfo& getConnectionInfo() const noexcept;
			bool getIsTickAsync() const noexcept;

		private:
			std::unique_ptr<MqttClientImpl> m_impl{ nullptr };
		};
	}
}