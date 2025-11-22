#include <mqttClient/Model/SessionModel.h>
#include <mqttClient/Adapters/CustomLogger.h>
#include <cleanMqtt/Logger/LoggerInstance.h>

SessionModel::SessionModel()
{
	topicModel = std::make_shared<TopicsModel>();
	publishModel = std::make_shared<PublishModel>();
	messagesModel = std::make_shared<MessagesModel>();
	outputModel = std::make_shared<OutputModel>();
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
	topicModel->reset();

	auto connectArgsCpy{ connectArgs };
	auto connectAddressCpy{ connectAddress };

	/**
	 * Create MQTT Client.
	 */
	cleanMqtt::DefaultEnvironmentFactory factory;
	cleanMqtt::MqttClientOptions options{ useTickAsync ? cleanMqtt::TickMode::ASYNC : cleanMqtt::TickMode::SYNC };

	//Set up customer logger
	std::weak_ptr<OutputModel> weakOutputMdl{ outputModel };
	cleanMqtt::setLogger(new CustomLogger(weakOutputMdl));

	m_mqttClient = new cleanMqtt::mqtt::MqttClient{ factory.createEnvironment(), options };

	/**
	 * Try to connect.
	 */
	if(!m_mqttClient->connect(std::move(connectArgsCpy), std::move(connectAddressCpy)).noError())
	{
		delete m_mqttClient;
		m_mqttClient = nullptr;
		return;
	}

	/**
	 * Set MQTT client for models.
	 */
	topicModel->setMqttClient(m_mqttClient);
	publishModel->setMqttClient(m_mqttClient);
	messagesModel->setMqttClient(m_mqttClient);
	outputModel->setMqttClient(m_mqttClient);

	/**
	 * Register to connection events.
	 */
	m_mqttClient->onConnectEvent().add([this](const cleanMqtt::mqtt::ConnectEventDetails& details, const cleanMqtt::mqtt::ConnectAck& ack)
		{
			isMqttConnected = details.isSuccessful;

			if (details.isSuccessful)
			{
				const std::string reasonCode{ "MQTT Reason Code: " + std::to_string(static_cast<std::underlying_type_t<cleanMqtt::mqtt::ConnectReasonCode>>(ack.getVariableHeader().reasonCode)) };
				connectionFailureReason = details.hasReceivedAck ? reasonCode.c_str() : details.error.errorMsg;
			}
			else
			{
				connectionFailureReason = "";
			}
		});

	/**
	 * Register to disconnection events.
	 */
	m_mqttClient->onDisconnectEvent().add([this](const cleanMqtt::mqtt::DisconnectEventDetails& details)
		{
			isMqttConnected = false;

			if(details.isBrokerInduced)
			{
				const std::string reasonCode{ "MQTT Reason Code: " + std::to_string(static_cast<std::underlying_type_t<cleanMqtt::mqtt::DisconnectReasonCode>>(details.reasonCode)) };
				disconnectioReason = reasonCode.c_str();
			}
			else
			{
				disconnectioReason = details.error.errorMsg;
			}
		});
}

void SessionModel::disconnect()
{
	if (m_mqttClient != nullptr)
	{
		m_mqttClient->disconnect();
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