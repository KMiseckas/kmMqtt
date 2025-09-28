#ifndef INCLUDE_CLEANMQTT_SETTINGS_H
#define INCLUDE_CLEANMQTT_SETTINGS_H

#include <cleanMqtt/Interfaces/ILogger.h>

namespace cleanMqtt
{
	namespace settings
	{
		const interfaces::ILogger* const getLogger();

		void setLogger(interfaces::ILogger* logger, bool deleteOld = true);
	}
}

#endif //INCLUDE_CLEANMQTT_SETTINGS_H