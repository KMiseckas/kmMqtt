#pragma once

#include <cleanMqtt/Interfaces/ILogger.h>

namespace cleanMqtt
{
	namespace logger
	{
		class CleanLogger : public interfaces::ILogger
		{
		public:
			CleanLogger() noexcept;

			~CleanLogger() override;

			void Log(const LogLevel logLvl, const char* const msg) const noexcept override;
			void Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept override;
		};
	}
}
