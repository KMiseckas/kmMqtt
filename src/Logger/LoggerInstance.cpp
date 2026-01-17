// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Logger/LoggerInstance.h>

namespace kmMqtt
{
	ILogger* m_logger{ nullptr };

	ILogger* getLogger()
	{
		return m_logger;
	}

	void setLogger(ILogger* logger, const bool deleteOld)
	{
		if (deleteOld && m_logger != nullptr)
		{
			delete m_logger;
		}

		m_logger = logger;
	}
}