#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Enums/PayloadFormatIndicator.h>
#include <kmMqtt/Mqtt/Enums/Qos.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <string>
#include <memory>
#include <map>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API PublishOptions
		{
			PublishOptions() noexcept
			{
			}

			PublishOptions(const PublishOptions& other)
				: responseTopic{ other.responseTopic },
				topicAlias{ other.topicAlias },
				messageExpiryInterval{ other.messageExpiryInterval },
				addMessageExpiryInterval{ other.addMessageExpiryInterval },
				payloadFormatIndicator{ other.payloadFormatIndicator },
				retain{ other.retain },
				qos{ other.qos },
				userProperties{ other.userProperties }
			{
				if (other.correlationData != nullptr)
				{
					correlationData = std::make_unique<BinaryData>(other.correlationData->size(), other.correlationData->bytes());
				}
			}

			PublishOptions& operator=(const PublishOptions& other)
			{
				if (this == &other)
				{
					return* this;
				}

				if (other.correlationData != nullptr)
				{
					correlationData = std::make_unique<BinaryData>(other.correlationData->size(), other.correlationData->bytes());
				}

				responseTopic = other.responseTopic;
				topicAlias = other.topicAlias;
				messageExpiryInterval = other.messageExpiryInterval;
				addMessageExpiryInterval = other.addMessageExpiryInterval;
				payloadFormatIndicator = other.payloadFormatIndicator;
				retain = other.retain;
				qos = other.qos;
				userProperties = other.userProperties;

				return *this;
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
				responseTopic = std::move(other.responseTopic);
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

			std::unique_ptr<BinaryData> correlationData{ nullptr };
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

#endif //INCLUDE_KMMQTT_MQTT_PARAMS_PUBLISHOPTIONS_H 
