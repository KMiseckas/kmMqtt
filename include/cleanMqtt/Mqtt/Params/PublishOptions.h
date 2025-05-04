#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Enums/PayloadFormatIndicator.h>
#include <cleanMqtt/Mqtt/Enums/Qos.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API PublishOptions
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(PublishOptions)

			PublishOptions() noexcept
			{
			}

			PublishOptions(PublishOptions&& other) noexcept
				: correlationData{ std::move(other.correlationData)},
				responseTopic{other.responseTopic},
				topicAlias{other.topicAlias},
				messageExpiryInterval{other.messageExpiryInterval},
				addMessageExpiryInterval{other.addMessageExpiryInterval},
				payloadFormatIndicator{other.payloadFormatIndicator },
				retain{other.retain},
				qos{ other.qos },
				userProperties{ std::move(other.userProperties) }
			{
				other.correlationData = nullptr;
			}

			~PublishOptions() noexcept
			{
				
			}

			PublishOptions& operator=(PublishOptions&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				correlationData = std::move(other.correlationData);
				responseTopic = std::move(responseTopic);
				topicAlias = other.topicAlias;
				messageExpiryInterval = other.messageExpiryInterval;
				addMessageExpiryInterval = other.addMessageExpiryInterval;
				payloadFormatIndicator = other.payloadFormatIndicator;
				retain = other.retain;
				qos = other.qos;
				userProperties = std::move(other.userProperties);

				other.correlationData = nullptr;
				other.userProperties.clear();

				return *this;
			}

			std::unique_ptr<packets::BinaryData> correlationData{ nullptr };
			std::string responseTopic{ "" };
			std::uint16_t topicAlias{ 0U };
			std::uint32_t messageExpiryInterval{ 0U };
			bool addMessageExpiryInterval{ false };
			PayloadFormatIndicator payloadFormatIndicator{ PayloadFormatIndicator::UTF8 };
			bool retain{ false };
			Qos qos{ Qos::QOS_0 };
			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H 
