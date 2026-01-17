// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H
#define INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H

#include <mqttClient/Events/Event.h>
#include <kmMqtt/Mqtt/Params/PublishOptions.h>

namespace events 
{
	struct ExitAppRequestEvent : public Event
	{
	};

	struct RebootRequestEvent : public Event
	{
	};

	struct PublishMessageEvent : public Event
	{
		struct Result
		{
			bool success;
			std::string errorMessage;
		};

		Result result;
		std::string topic;
		std::string message;
		kmMqtt::mqtt::PublishOptions options;
	};
}

#endif //INCLUDE_MQTTCLIENT_EVENTS_APPLICATIONEVENTS_H 
