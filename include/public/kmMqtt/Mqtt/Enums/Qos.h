#ifndef INCLUDE_KMMQTT_MQTT_QOS_H
#define INCLUDE_KMMQTT_MQTT_QOS_H

#include <cstdint>

namespace kmMqtt 
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

#endif //INCLUDE_KMMQTT_MQTT_QOS_H 