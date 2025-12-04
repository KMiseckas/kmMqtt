#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBCOMPOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBCOMPOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubCompOptions
		{
			PubCompOptions() noexcept
			{
			}

			PubCompOptions(const PubCompOptions& other)
				: reasonString{ other.reasonString },
				userProperties{ other.userProperties }
			{

			}

			PubCompOptions& operator=(const PubCompOptions& other)
			{
				if (this == &other)
				{
					return*this;
				}

				reasonString = other.reasonString;
				userProperties = other.userProperties;

				return *this;
			}

			PubCompOptions(PubCompOptions&& other) noexcept
				: reasonString{ std::move(other.reasonString) },
				userProperties{ std::move(other.userProperties) }
			{
			}

			~PubCompOptions() noexcept
			{

			}

			PubCompOptions& operator=(PubCompOptions&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				reasonString = std::move(other.reasonString);
				userProperties = std::move(other.userProperties);

				other.userProperties.clear();

				return *this;
			}

			std::string reasonString{ "" };
			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBCOMPOPTIONS_H 
