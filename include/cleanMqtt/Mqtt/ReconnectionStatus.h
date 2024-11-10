#ifndef INCLUDE_CLEANMQTT_MQTT_RECONNECTIONSTATUS_H
#define INCLUDE_CLEANMQTT_MQTT_RECONNECTIONSTATUS_H

#include <cstdint>

namespace cleanMqtt
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

#endif //INCLUDE_CLEANMQTT_MQTT_RECONNECTIONSTATUS_H 
