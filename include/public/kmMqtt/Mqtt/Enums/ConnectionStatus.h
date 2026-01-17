#ifndef INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H
#define INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H

#include <cstdint>

namespace kmMqtt 
{
	namespace mqtt
	{
		enum class ConnectionStatus : std::uint8_t
		{
			DISCONNECTED,
			CONNECTED,
			CONNECTING,
			RECONNECT,
			RECONNECTING,
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_CONNECTIONSTATUS_H 
