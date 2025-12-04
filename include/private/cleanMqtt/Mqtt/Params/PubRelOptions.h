#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRELOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRELOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubRelOptions
		{
			PubRelOptions() noexcept
			{
			}

			PubRelOptions(const PubRelOptions& other)
				: reasonString{ other.reasonString },
				userProperties{ other.userProperties }
			{

			}

			PubRelOptions& operator=(const PubRelOptions& other)
			{
				if (this == &other)
				{
					return*this;
				}

				reasonString = other.reasonString;
				userProperties = other.userProperties;

				return *this;
			}

			PubRelOptions(PubRelOptions&& other) noexcept
				: reasonString{ std::move(other.reasonString) },
				userProperties{ std::move(other.userProperties) }
			{
			}

			~PubRelOptions() noexcept
			{

			}

			PubRelOptions& operator=(PubRelOptions&& other) noexcept
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

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBRELOPTIONS_H 
