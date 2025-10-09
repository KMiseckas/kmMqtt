#ifndef INCLUDE_CLEANMQTT_LOGGERINSTANCE_H
#define INCLUDE_CLEANMQTT_LOGGERINSTANCE_H

#include <cleanMqtt/Interfaces/ILogger.h>

namespace cleanMqtt
{
	const ILogger* const getLogger();

	void setLogger(ILogger* logger, bool deleteOld = true);
}

#endif //INCLUDE_CLEANMQTT_LOGGERINSTANCE_H