#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H

#include <cleanMqtt/GlobalMacros.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API SubscribeOptions
		{
			SubscribeOptions() noexcept
			{
			}

			SubscribeOptions(const SubscribeOptions& other)
				: subscribeIdentifier{ other.subscribeIdentifier },
				userProperties{ other.userProperties }
			{
			}

			SubscribeOptions& operator=(const SubscribeOptions& other)
			{
				if (this == &other)
				{
					return *this;
				}

				subscribeIdentifier = other.subscribeIdentifier;
				userProperties = other.userProperties;

				return *this;
			}

			SubscribeOptions(SubscribeOptions&& other) noexcept 
				: subscribeIdentifier{ other.subscribeIdentifier }, 
				userProperties{ std::move(other.userProperties) }
			{
			}

			~SubscribeOptions() noexcept
			{
			}

			SubscribeOptions& operator=(SubscribeOptions&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				subscribeIdentifier = other.subscribeIdentifier;
				userProperties = std::move(other.userProperties);

				other.userProperties.clear();

				return *this;
			}
			 
			VariableByteInteger subscribeIdentifier{ 0 }; //Use 0 to ignore subscribtion identifier property.
			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H
