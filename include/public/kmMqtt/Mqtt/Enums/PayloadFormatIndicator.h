#ifndef INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H
#define INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H

#include <cstdint>

namespace kmMqtt
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

#endif //INTERFACE_KMMQTT_MQTT_PARAMS_ENUMS_PAYLOADFORMATINDICATOR_H
