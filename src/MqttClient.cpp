#include <kmMqtt/MqttClient.h>
#include <kmMqtt/Mqtt/MqttClientImpl.h>

namespace kmMqtt
{
	namespace mqtt
	{
		MqttClient::MqttClient()
		{
			auto envFactory{ DefaultEnvironmentFactory() };
			auto env{ envFactory.createEnvironment() };
			m_impl = std::make_unique<MqttClientImpl>(env, MqttClientOptions{});
			delete env;
		}

		MqttClient::MqttClient(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions)
			: m_impl(std::make_unique<MqttClientImpl>(env, clientOptions))
		{
			if (clientOptions.getTickMode() == TickMode::ASYNC)
			{
				m_impl->tickAsync();
			}
		}

		MqttClient::~MqttClient() = default;

		ReqResult MqttClient::connect(ConnectArgs&& args, ConnectAddress&& address) noexcept
		{
			return m_impl->connect(std::move(args), std::move(address));
		}

		ReqResult MqttClient::publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept
		{
			return m_impl->publish(topic, std::move(payload), std::move(options));
		}

		ReqResult MqttClient::subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept
		{
			return m_impl->subscribe(topics, std::move(options));
		}

		ReqResult MqttClient::unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept
		{
			return m_impl->unSubscribe(topics, std::move(options));
		}

		ReqResult MqttClient::disconnect(DisconnectArgs&& args) noexcept
		{
			return m_impl->disconnect(std::move(args));
		}

		ClientError MqttClient::shutdown() noexcept
		{
			return m_impl->shutdown();
		}

		ClientError MqttClient::tick() noexcept
		{
			return m_impl->tick();
		}

		ErrorEvent& MqttClient::onErrorEvent() noexcept
		{
			return m_impl->onErrorEvent();
		}

		ConnectEvent& MqttClient::onConnectEvent() noexcept
		{
			return m_impl->onConnectEvent();
		}

		DisconnectEvent& MqttClient::onDisconnectEvent() noexcept
		{
			return m_impl->onDisconnectEvent();
		}

		ReconnectEvent& MqttClient::onReconnectEvent() noexcept
		{
			return m_impl->onReconnectEvent();
		}

		PublishEvent& MqttClient::onPublishEvent() noexcept
		{
			return m_impl->onPublishEvent();
		}

		PublishCompletedEvent& MqttClient::onPublishCompletedEvent() noexcept
		{
			return m_impl->onPublishCompletedEvent();
		}

		SubscribeAckEvent& MqttClient::onSubscribeAckEvent() noexcept
		{
			return m_impl->onSubscribeAckEvent();
		}

		UnSubscribeAckEvent& MqttClient::onUnSubscribeAckEvent() noexcept
		{
			return m_impl->onUnSubscribeAckEvent();
		}

		ConnectionStatus MqttClient::getConnectionStatus() const noexcept
		{
			return m_impl->getConnectionStatus();
		}

		const MqttConnectionInfo& MqttClient::getConnectionInfo() const noexcept
		{
			return m_impl->getConnectionInfo();
		}

		bool MqttClient::getIsTickAsync() const noexcept
		{
			return m_impl->getIsTickingAsync();
		}
	}
}