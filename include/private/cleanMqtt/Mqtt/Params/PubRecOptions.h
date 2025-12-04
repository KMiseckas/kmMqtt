#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRECOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRECOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubRecOptions
		{
			PubRecOptions() noexcept
			{
			}

			PubRecOptions(const PubRecOptions& other)
				: reasonString{ other.reasonString },
				userProperties{ other.userProperties }
			{

			}

			PubRecOptions& operator=(const PubRecOptions& other)
			{
				if (this == &other)
				{
					return*this;
				}

				reasonString = other.reasonString;
				userProperties = other.userProperties;

				return *this;
			}

			PubRecOptions(PubRecOptions&& other) noexcept
				: reasonString{ std::move(other.reasonString) },
				userProperties{ std::move(other.userProperties) }
			{
			}

			~PubRecOptions() noexcept
			{

			}

			PubRecOptions& operator=(PubRecOptions&& other) noexcept
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

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRECOPTIONS_H 
