#ifndef INCLUDE_KMMQTT_LOGGERINSTANCE_H
#define INCLUDE_KMMQTT_LOGGERINSTANCE_H

#include <kmMqtt/Interfaces/ILogger.h>

namespace kmMqtt
{
	ILogger* getLogger();

	void setLogger(ILogger* logger, bool deleteOld = true);
}

#endif //INCLUDE_KMMQTT_LOGGERINSTANCE_H