#ifndef INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H
#define INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H

#include <cstdint>

namespace cleanMqtt
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

#endif //INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_LOCATORTYPE_H
