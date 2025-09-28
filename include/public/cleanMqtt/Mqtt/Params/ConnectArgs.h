#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_CONNECTARGS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_CONNECTARGS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Enums/Qos.h>
#include <cleanMqtt/Mqtt/Enums/MqttVersion.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cleanMqtt/Mqtt/Enums/PayloadFormatIndicator.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API Will
		{
			Will(const std::string& topic) noexcept
				:willTopic{ topic }
			{
			}

			Will(Will&& other) noexcept
				: willQos{other.willQos},
				retainWillMessage{other.retainWillMessage},
				willDelayInterval{other.willDelayInterval},
				messageExpiryInterval{other.messageExpiryInterval},
				contentType{ std::move(other.contentType)},
				responseTopic{ std::move(other.responseTopic)},
				correlationData{std::move(other.correlationData)},
				willTopic{ std::move(other.willTopic)},
				payloadFormat{other.payloadFormat},
				payload{std::move(other.payload)},
				userProperties{std::move(other.userProperties)}
			{
				other.correlationData = nullptr;
				other.payload = nullptr;
			}

			Will(const Will& other) noexcept
				: willQos{ other.willQos },
				retainWillMessage{ other.retainWillMessage },
				willDelayInterval{ other.willDelayInterval },
				messageExpiryInterval{ other.messageExpiryInterval },
				contentType{ other.contentType },
				responseTopic{ other.responseTopic },
				willTopic{ other.willTopic },
				payloadFormat{ other.payloadFormat },
				userProperties{ other.userProperties }
			{
				if (other.correlationData != nullptr)
				{
					packets::BinaryData copyCorrelationData = *other.correlationData.get();
					correlationData = std::make_unique<packets::BinaryData>(std::move(copyCorrelationData));
				}

				if (other.payload != nullptr)
				{
					packets::BinaryData payloadData = *other.payload.get();
					payload = std::make_unique<packets::BinaryData>(std::move(payloadData));
				}
			}

			~Will() noexcept
			{
				correlationData = nullptr;
				payload = nullptr;
			}

			Will& operator=(Will&& other) noexcept
			{
				if (this != &other)
				{
					return *this;
				}

				willQos = other.willQos;
				retainWillMessage = other.retainWillMessage;
				willDelayInterval = other.willDelayInterval;
				messageExpiryInterval = other.messageExpiryInterval;
				contentType = std::move(other.contentType);
				responseTopic = std::move(other.responseTopic);
				correlationData = std::move(other.correlationData);
				willTopic = std::move(other.willTopic);
				payloadFormat = other.payloadFormat;
				payload = std::move(other.payload);
				userProperties = std::move(other.userProperties);

				other.correlationData = nullptr;
				other.payload = nullptr;

				return *this;
			}

			Will& operator=(const Will& other) noexcept
			{
				if (this != &other)
				{
					return *this;
				}


				willQos = other.willQos;
				retainWillMessage = other.retainWillMessage;
				willDelayInterval = other.willDelayInterval;
				messageExpiryInterval = other.messageExpiryInterval;
				contentType = other.contentType;
				responseTopic = other.responseTopic;
				correlationData = other.correlationData == nullptr ? nullptr : std::make_unique<packets::BinaryData>(*other.correlationData.get());
				willTopic = other.willTopic;
				payloadFormat = other.payloadFormat;
				payload = other.payload == nullptr ? nullptr : std::make_unique<packets::BinaryData>(*other.payload.get());
				userProperties = other.userProperties;


				return *this;
			}

			Qos willQos{ Qos::QOS_0 };
			bool retainWillMessage{ false };
			std::uint32_t willDelayInterval{ 0U };
			std::uint32_t messageExpiryInterval{ 0U };
			std::string contentType{};
			std::string responseTopic;
			std::unique_ptr<packets::BinaryData> correlationData{ nullptr };
			std::string willTopic;
			PayloadFormatIndicator payloadFormat{ PayloadFormatIndicator::BINARY };
			std::unique_ptr<packets::BinaryData> payload{ nullptr };
			std::map<std::string, std::string> userProperties;
		};

		struct PUBLIC_API ConnectArgs
		{
			ConnectArgs(const std::string& clientId) noexcept
				:clientId{clientId}
			{
			}

			ConnectArgs(ConnectArgs&& other) noexcept
				: cleanStart{ other.cleanStart },
				clientId{ std::move(other.clientId) },
				will{ std::move(other.will) },
				username{ std::move(other.username) },
				password{ std::move(other.password) },
				extendedAuthenticationMethod{ std::move(other.extendedAuthenticationMethod) },
				extendedAuthenticationData{ std::move(other.extendedAuthenticationData) },
				version{ other.version },
				protocolName{ std::move(other.protocolName) },
				keepAliveInSec{ other.keepAliveInSec },
				sessionExpiryInterval{ other.sessionExpiryInterval },
				receiveMaximum{ other.receiveMaximum },
				maximumTopicAliases{ other.maximumTopicAliases },
				requestResponseInformation{ other.requestResponseInformation },
				requestProblemInformation{ other.requestProblemInformation },
				userProperties{ std::move(other.userProperties) }
			{
				other.will = nullptr;
				other.extendedAuthenticationData = nullptr;
			}

			ConnectArgs(const ConnectArgs& other) noexcept
				: cleanStart{ other.cleanStart },
				clientId{ other.clientId },
				will{ other.will == nullptr ? nullptr : std::make_unique<Will>(*other.will.get()) },
				username{ other.username },
				password{ other.password },
				extendedAuthenticationMethod{ other.extendedAuthenticationMethod },
				version{ other.version },
				protocolName{ other.protocolName },
				keepAliveInSec{ other.keepAliveInSec },
				sessionExpiryInterval{ other.sessionExpiryInterval },
				receiveMaximum{ other.receiveMaximum },
				maximumTopicAliases{ other.maximumTopicAliases },
				requestResponseInformation{ other.requestResponseInformation },
				requestProblemInformation{ other.requestProblemInformation },
				userProperties{ other.userProperties }
			{
				if (other.extendedAuthenticationData != nullptr)
				{
					auto copy = *other.extendedAuthenticationData.get();
					extendedAuthenticationData = std::make_unique<packets::BinaryData>(std::move(copy));
				}
			}

			~ConnectArgs() noexcept
			{
				will = nullptr;
				extendedAuthenticationData = nullptr;
			}

			ConnectArgs& operator=(ConnectArgs&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				cleanStart = other.cleanStart;
				clientId = std::move(other.clientId);
				will = std::move(other.will);
				username = std::move(other.username);
				password = std::move(other.password);
				extendedAuthenticationMethod = std::move(other.extendedAuthenticationMethod);
				extendedAuthenticationData = std::move(other.extendedAuthenticationData);
				version = other.version;
				protocolName = std::move(other.protocolName);
				keepAliveInSec = other.keepAliveInSec;
				sessionExpiryInterval = other.sessionExpiryInterval;
				receiveMaximum = other.receiveMaximum;
				maximumTopicAliases = other.maximumTopicAliases;
				requestResponseInformation = other.requestResponseInformation;
				requestProblemInformation = other.requestProblemInformation;
				userProperties = std::move(other.userProperties);

				other.will = nullptr;
				other.extendedAuthenticationData = nullptr;

				return *this;
			}

			ConnectArgs& operator=(const ConnectArgs& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				cleanStart = other.cleanStart;
				clientId = other.clientId;
				will = other.will == nullptr ? nullptr : std::make_unique<Will>(*other.will.get());
				username = other.username;
				password = other.password;
				extendedAuthenticationMethod = other.extendedAuthenticationMethod;
				extendedAuthenticationData = other.extendedAuthenticationData != nullptr ? std::make_unique<packets::BinaryData>(*other.extendedAuthenticationData.get()) : nullptr;
				version = other.version;
				protocolName = other.protocolName;
				keepAliveInSec = other.keepAliveInSec;
				sessionExpiryInterval = other.sessionExpiryInterval;
				receiveMaximum = other.receiveMaximum;
				maximumTopicAliases = other.maximumTopicAliases;
				requestResponseInformation = other.requestResponseInformation;
				requestProblemInformation = other.requestProblemInformation;
				userProperties = other.userProperties;

				return *this;
			}

			bool cleanStart{ true };
			std::string clientId;
			std::unique_ptr<Will> will{ nullptr };
			std::string username;
			std::string password;
			std::string extendedAuthenticationMethod;
			std::unique_ptr<packets::BinaryData> extendedAuthenticationData{nullptr};
			MqttVersion version{ MqttVersion::MQTT_5_0 };
			std::string protocolName{ "MQTT" };
			std::uint16_t keepAliveInSec{ 60U };
			std::uint32_t sessionExpiryInterval{ 0U };
			std::uint16_t receiveMaximum{ 0U };
			std::uint32_t maximumPacketSize{ 0U };
			std::uint16_t maximumTopicAliases{ 0U };
			bool requestResponseInformation{ false };
			bool requestProblemInformation{ true };

			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_CONNECTARGS_H 
