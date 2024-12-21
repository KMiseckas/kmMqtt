#ifndef INCLUDE_MQTTCLIENT_EVENTS_EVENT_H
#define INCLUDE_MQTTCLIENT_EVENTS_EVENT_H

namespace events
{
	struct Event
	{
		virtual ~Event() = default;
	};
}
#endif //INCLUDE_MQTTCLIENT_EVENTS_EVENT_H 