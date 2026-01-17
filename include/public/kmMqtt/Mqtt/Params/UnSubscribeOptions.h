// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>

namespace kmMqtt
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

#endif //INCLUDE_KMMQTT_MQTT_PARAMS_UNSUBSCRIBEOPTIONS_H
