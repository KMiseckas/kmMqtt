// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_MQTTCLIENT_MODEL_VIEWMODEL_H
#define INCLUDE_MQTTCLIENT_MODEL_VIEWMODEL_H

#include <memory>

class ViewModel 
{
public:
	ViewModel() noexcept = default;
	virtual ~ViewModel() = default;
};

#endif //INCLUDE_MQTTCLIENT_MODEL_VIEWMODEL_H 
