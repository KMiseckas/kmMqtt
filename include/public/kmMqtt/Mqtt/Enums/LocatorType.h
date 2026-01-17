// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H
#define INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class LocatorType : std::uint8_t
		{
			UNKNOWN = 0U,
			IP4,
			IP6,
			HOSTNAME
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H
