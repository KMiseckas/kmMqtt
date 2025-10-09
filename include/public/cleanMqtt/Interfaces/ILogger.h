#ifndef INCLUDE_CLEANMQTT_INTERFACES_ILOGGER_H
#define INCLUDE_CLEANMQTT_INTERFACES_ILOGGER_H

#include <cleanMqtt/GlobalMacros.h>
#include <cstdint>
#include <exception>

namespace cleanMqtt
{
	enum class LogLevel : std::uint8_t
	{
		Trace = 0U,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
	};

	class PUBLIC_API ILogger
	{
	public:
		ILogger() noexcept
		{
		};

		virtual ~ILogger()
		{
		};

		virtual void Log(const LogLevel logLvl, const char* const msg) const noexcept = 0;
		virtual void Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept = 0;

		void setLowestLogLevel(const LogLevel logLvl)
		{
			m_lowestLogLevel = logLvl;
		}

		constexpr LogLevel getLowestLogLevel() const
		{
			return m_lowestLogLevel;
		}

	protected:

		LogLevel m_lowestLogLevel{ LogLevel::Info };
	};
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_ILOGGER_H