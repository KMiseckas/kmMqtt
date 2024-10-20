#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_CONNECTARGS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_CONNECTARGS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Qos.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class PayloadFormatIndicator : std::uint8_t
		{
			BINARY = 0,
			UTF8 = 1,
		};

		struct PUBLIC_API Will
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(Will)

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
				correlationDataSize{other.correlationDataSize},
				willTopic{ std::move(other.willTopic)},
				payloadFormat{other.payloadFormat},
				payload{std::move(other.payload)},
				payloadSize{other.payloadSize},
				userProperties{std::move(other.userProperties)}
			{
				other.correlationData = nullptr;
				other.payload = nullptr;
			}

			~Will() noexcept
			{
				correlationData = nullptr;
				payload = nullptr;
			}

			Will& operator=(Will& other) noexcept
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
				correlationDataSize = other.correlationDataSize;
				willTopic = std::move(other.willTopic);
				payloadFormat = other.payloadFormat;
				payload = std::move(other.payload);
				payloadSize = other.payloadSize;
				userProperties = std::move(other.userProperties);

				other.correlationData = nullptr;
				other.payload = nullptr;

				return *this;
			}

			Qos willQos{ Qos::QOS_0 };
			bool retainWillMessage{ false };
			std::uint32_t willDelayInterval{ 0U };
			std::uint32_t messageExpiryInterval{ 0U };
			std::string contentType{};
			std::string responseTopic;
			std::unique_ptr<std::uint8_t*> correlationData{ nullptr };
			std::uint16_t correlationDataSize{ 0U };
			std::string willTopic;
			PayloadFormatIndicator payloadFormat;
			std::unique_ptr<std::uint8_t*> payload{ nullptr };
			std::uint16_t payloadSize{ 0U };
			std::map<std::string, std::string> userProperties;
		};

		struct PUBLIC_API ConnectArgs
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ConnectArgs)

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

			bool cleanStart{ true };
			std::string clientId;
			std::unique_ptr<Will> will{ nullptr };
			std::string username;
			std::string password;
			std::string extendedAuthenticationMethod;
			std::unique_ptr<std::uint8_t*> extendedAuthenticationData{ nullptr };

			MqttVersion version{ MqttVersion::MQTT_5_0 };
			std::string protocolName{ "MQTT" };
			std::uint16_t keepAliveInSec{ 0U };
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
