#ifndef INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H
#define INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H

#include <mqttClient/Events/Event.h>

namespace events 
{
	struct ExitAppRequestEvent : public Event
	{
	};

	struct RebootRequestEvent : public Event
	{
	};
}

#endif //INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H 
