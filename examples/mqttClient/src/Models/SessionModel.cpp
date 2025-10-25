#include <mqttClient/Model/SessionModel.h>
#include <cleanMqtt/Environments/DefaultWinEnv.h>

SessionModel::SessionModel()
{
}

SessionModel::~SessionModel()
{
	if (m_mqttClient != nullptr)
	{
		m_mqttClient->shutdown();
		delete m_mqttClient;
	}
}

void SessionModel::setIndex(std::uint8_t index) noexcept
{
	m_index = index;
}

void SessionModel::setName(const char* name) noexcept
{
	m_name = name;
}

std::uint8_t SessionModel::getIndex() const noexcept
{
	return m_index;
}

const std::string& SessionModel::getName() const noexcept
{
	return m_name;
}

void SessionModel::connect()
{
	failedLastConnect = false;

	cleanMqtt::DefaultEnvironmentFactory factory;
	m_mqttClient = new cleanMqtt::mqtt::MqttClient{ factory.createEnvironment(), useTickAsync};

	auto connectArgsCpy{ connectArgs };
	auto connectAddressCpy{ connectAddress };

	m_mqttClient->connect(std::move(connectArgsCpy), std::move(connectAddressCpy));

	if(m_mqttClient->getConnectionStatus() != cleanMqtt::mqtt::ConnectionStatus::CONNECTING)
	{
		delete m_mqttClient;
		m_mqttClient = nullptr;

		failedLastConnect = true;
	}
}

void SessionModel::disconnect()
{
	if (m_mqttClient != nullptr)
	{
		m_mqttClient->disconnect(std::move(disconnectArgs));
	}
}

void SessionModel::cancel()
{
	if (m_mqttClient != nullptr)
	{
		m_mqttClient->shutdown();
	}
}

void SessionModel::tickMqtt()
{
	if (m_mqttClient != nullptr)
	{
		if (m_mqttClient->getIsTickAsync())
		{
			return; //Ticking internally in own thread.
		}

		m_mqttClient->tick();
	}
}

cleanMqtt::mqtt::ConnectionStatus SessionModel::getConnectionStatus() const
{
	if (m_mqttClient != nullptr)
	{
		return m_mqttClient->getConnectionStatus();
	}
	else
	{
		return cleanMqtt::mqtt::ConnectionStatus::DISCONNECTED;
	}
}