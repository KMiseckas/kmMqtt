#ifndef INCLUDE_MQTTCLIENT_MODEL_SESSIONMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_SESSIONMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <cleanMqtt/Config.h>
#include <cleanMqtt/Mqtt/Params/ConnectArgs.h>
#include <cleanMqtt/Mqtt/Params/ConnectAddress.h>
#include <cleanMqtt/Mqtt/Params/DisconnectArgs.h>
#include <string>
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Mqtt/Enums/ConnectionStatus.h>

class SessionModel : public ViewModel
{
public:
	SessionModel();
	~SessionModel() override;

	void setIndex(std::uint8_t index) noexcept;
	void setName(const char* name) noexcept;

	std::uint8_t getIndex() const noexcept;
	const std::string& getName() const noexcept;

	void connect();
	void disconnect();
	void cancel();
	void tickMqtt();

	cleanMqtt::mqtt::ConnectionStatus getConnectionStatus() const;

	cleanMqtt::Config mqttConfig;
	cleanMqtt::mqtt::ConnectAddress connectAddress;
	cleanMqtt::mqtt::ConnectArgs connectArgs{ "Client ID" };
	cleanMqtt::mqtt::DisconnectArgs disconnectArgs{ false, true };

	bool useTickAsync{ false };

	struct UIData
	{
		int tickAsync{ 0 };

		char addressBuffer[512]{ "" };
		char usernameBuffer[512]{ "" };
		char passwordBuffer[512]{ "" };
		char clientIdBuffer[512]{ "Client ID" };
		int cleanStartSelectedOption{ 1 };
		bool showingAdvancedConnectOptions{ false };

		int keepAliveSec{ 60U };
		int expiryInterval{ 0U };
		int receiveMaximum{ 0U };
		int maxTopicAliases{ 0U };

		char extendedAuthMethod[512]{ "" };
		char extendedAuthData[2048]{ "" };

		bool showingWillOptions{ false };
		int willDelayInterval{ 0U };
		int willMsgExpiryInterval{ 0U };

		char willContentType[512]{ "" };
		char willPayload[16384]{ "" };
		char willTopic[512]{ "" };
		char willResponseTopic[512]{ "" };
	} uiData;

	bool failedLastConnect{ false };
private:
	std::uint8_t m_index{ 0 };
	std::string m_name{ "New Session" };

	cleanMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
};

#endif //INCLUDE_MQTTCLIENT_MODEL_SESSIONMODEL_H 
