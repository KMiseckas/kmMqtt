// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Packets/DataTypes.h>
#include <map>

namespace kmMqtt
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
			 
			VariableByteInteger subscribeIdentifier;
			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PARAMS_SUBSCRIBEOPTIONS_H
