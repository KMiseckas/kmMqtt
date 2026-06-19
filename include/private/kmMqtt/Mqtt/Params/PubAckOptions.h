// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_PUBACKOPTIONS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_PUBACKOPTIONS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/STL/KmString.h>
#include <memory>
#include <kmMqtt/STL/KmMap.h>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief Options for PUBACK packet creation.
		 */
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

			kmStd::string reasonString{ "" };
			kmStd::map<kmStd::string, kmStd::string> userProperties;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PARAMS_PUBACKOPTIONS_H 
