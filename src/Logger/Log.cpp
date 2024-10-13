#include <cleanMqtt/Settings.h>

namespace cleanMqtt
{
	void cleanMqtt::Log(const LogLevel logLvl, const char* const msg) noexcept
	{
		if (logLvl < settings::getLogger()->getLowestLogLevel())
		{
			return;
		}

		settings::getLogger()->Log(logLvl, msg);
	}

	void cleanMqtt::Log(const LogLevel logLvl, const char* const category, const char* const msg) noexcept
	{
		if (logLvl < settings::getLogger()->getLowestLogLevel())
		{
			return;
		}

		settings::getLogger()->Log(logLvl, category, msg);
	}

	void cleanMqtt::Exception(const LogLevel logLvl, const char* const category, const std::exception& e) noexcept(false)
	{
		if (logLvl < settings::getLogger()->getLowestLogLevel())
		{
			return;
		}

		settings::getLogger()->Log(logLvl, category, e.what());
		throw e;
	}
}
