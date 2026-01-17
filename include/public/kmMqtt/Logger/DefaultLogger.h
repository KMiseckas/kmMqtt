#ifndef KMMQTT_LOGGER_DEFAULTLOGGER_H
#define KMMQTT_LOGGER_DEFAULTLOGGER_H

#include "kmMqtt/GlobalMacros.h"
#include <kmMqtt/Interfaces/ILogger.h>

namespace kmMqtt
{
	class PUBLIC_API DefaultLogger : public ILogger
	{
	public:
		DefaultLogger() noexcept;

		~DefaultLogger() override;

		void Log(const LogLevel logLvl, const char* const msg) const noexcept override;
		void Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept override;
	};
}

#endif // KMMQTT_LOGGER_DEFAULTLOGGER_H
