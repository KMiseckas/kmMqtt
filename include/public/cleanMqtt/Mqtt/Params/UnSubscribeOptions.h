#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API UnSubscribeOptions
		{
			UnSubscribeOptions() noexcept
			{
			}

			UnSubscribeOptions(const UnSubscribeOptions& other)
				: userProperties{ other.userProperties }
			{
			}

			UnSubscribeOptions& operator=(const UnSubscribeOptions& other)
			{
				if (this == &other)
				{
					return *this;
				}

				userProperties = other.userProperties;

				return *this;
			}

			UnSubscribeOptions(UnSubscribeOptions&& other) noexcept
				: userProperties{ std::move(other.userProperties) }
			{
			}

			~UnSubscribeOptions() noexcept
			{
			}

			UnSubscribeOptions& operator=(UnSubscribeOptions&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				userProperties = std::move(other.userProperties);

				other.userProperties.clear();

				return *this;
			}

			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H
