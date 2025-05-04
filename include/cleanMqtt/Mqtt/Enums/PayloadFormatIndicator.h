#ifndef INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H
#define INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H

#include <cstdint>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class PayloadFormatIndicator : std::uint8_t
		{
			BINARY = 0,
			UTF8 = 1,
		};
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H
