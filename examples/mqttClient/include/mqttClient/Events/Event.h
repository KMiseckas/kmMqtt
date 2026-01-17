// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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