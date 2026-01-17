// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_LOGGERINSTANCE_H
#define INCLUDE_KMMQTT_LOGGERINSTANCE_H

#include <kmMqtt/Interfaces/ILogger.h>

namespace kmMqtt
{
	ILogger* getLogger();

	void setLogger(ILogger* logger, bool deleteOld = true);
}

#endif //INCLUDE_KMMQTT_LOGGERINSTANCE_H