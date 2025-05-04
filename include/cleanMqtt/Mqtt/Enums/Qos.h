#ifndef INCLUDE_CLEANMQTT_MQTT_QOS_H
#define INCLUDE_CLEANMQTT_MQTT_QOS_H

#include <cstdint>

namespace cleanMqtt 
{
	namespace mqtt
	{
		enum class Qos : std::uint8_t
		{
			QOS_0 = 0U,
			QOS_1 = 1U,
			QOS_2 = 2U,
		};
	}

}

#endif //INCLUDE_CLEANMQTT_MQTT_QOS_H 