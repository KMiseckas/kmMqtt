#ifndef INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H
#define INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H

#include <cstdint>

namespace kmMqtt
{
	namespace mqtt
	{
		enum class ReconnectionStatus : std::uint8_t
		{
			RECONNECTING,
			SUCCEEDED,
			FAILED,
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_RECONNECTIONSTATUS_H 
