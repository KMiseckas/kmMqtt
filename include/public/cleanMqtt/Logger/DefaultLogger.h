#ifndef CLEANMQTT_LOGGER_DEFAULTLOGGER_H
#define CLEANMQTT_LOGGER_DEFAULTLOGGER_H

#include <cleanMqtt/Interfaces/ILogger.h>

namespace cleanMqtt
{
	class DefaultLogger : public ILogger
	{
	public:
		DefaultLogger() noexcept;

		~DefaultLogger() override;

		void Log(const LogLevel logLvl, const char* const msg) const noexcept override;
		void Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept override;
	};
}

#endif // CLEANMQTT_LOGGER_DEFAULTLOGGER_H
