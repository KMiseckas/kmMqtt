#ifndef INCLUDE_CLEANMQTT_MQTT_CONNECTIONSTATUS_H
#define INCLUDE_CLEANMQTT_MQTT_CONNECTIONSTATUS_H

#include <cstdint>

namespace cleanMqtt 
{
	namespace mqtt
	{
		enum class ConnectionStatus : std::uint8_t
		{
			DISCONNECTED = 0U,
			CONNECTED = 1U,
			CONNECTING = 2U
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_CONNECTIONSTATUS_H 
