#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBACKOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBACKOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PubAckOptions
		{
			PubAckOptions() noexcept
			{
			}

			PubAckOptions(const PubAckOptions& other)
				: reasonString{ other.reasonString },
				userProperties{ other.userProperties }
			{

			}

			PubAckOptions& operator=(const PubAckOptions& other)
			{
				if (this == &other)
				{
					return*this;
				}

				reasonString = other.reasonString;
				userProperties = other.userProperties;

				return *this;
			}

			PubAckOptions(PubAckOptions&& other) noexcept
				: reasonString{ std::move(other.reasonString) },
				userProperties{ std::move(other.userProperties) }
			{
			}

			~PubAckOptions() noexcept
			{

			}

			PubAckOptions& operator=(PubAckOptions&& other) noexcept
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

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_PUBACKOPTIONS_H 
