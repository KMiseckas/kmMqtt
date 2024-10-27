#include <cleanMqtt/Logger/Log.h>
#include <cleanMqtt/Settings.h>

namespace cleanMqtt
{

namespace
{
#define TRACE 0
#define INFO 1
#define DEBUG 2
#define WARNING 3
#define ERROR 4
}

#ifndef ENABLE_LOGS
#define LOG_LEVEL 999
#endif //ENABLE_LOGS


#if LOG_LEVEL <= TRACE
	void LogTrace(const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Trace, msg);
	}

	void LogTrace(const char* category, const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Trace, category, msg);
	}
#else
	void LogTrace(const char* /*msg*/, ...) noexcept
	{
	}

	void LogTrace(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= TRACE

#if LOG_LEVEL <= INFO
	void LogInfo(const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Info, msg);
	}

	void LogInfo(const char* category, const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Info, category, msg);
	}
#else
	void LogInfo(const char* /*msg*/, ...) noexcept
	{
	}

	void LogInfo(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= INFO

#if LOG_LEVEL <= DEBUG
	void LogDebug(const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Debug, msg);
	}

	void LogDebug(const char* category, const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Debug, category, msg);
	}
#else
	void LogDebug(const char* /*msg*/, ...) noexcept
	{
	}

	void LogDebug(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= DEBUG

#if LOG_LEVEL <= WARNING
	void LogWarning(const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Warning, msg);
	}

	void LogWarning(const char* category, const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Warning, category, msg);
	}
#else
	void LogWarning(const char* /*msg*/, ...) noexcept
	{
	}

	void LogWarning(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= WARNING

#if LOG_LEVEL <= ERROR
	void LogError(const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Error, msg);
	}

	void LogError(const char* category, const char* msg, ...) noexcept
	{
		settings::getLogger()->Log(LogLevel::Error, category, msg);
	}
#else
	void LogError(const char* /*msg*/, ...) noexcept
	{
	}

	void LogError(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= ERROR

#ifdef ENABLE_THROW_ON_FATAL_LOG
	void LogException(const char* category, const std::exception& e) noexcept(false)
	{
		settings::getLogger()->Log(LogLevel::Fatal, category, e.what());
		throw e;
	}
#elif ENABLE_LOGS && LOG_LEVEL <= ERROR
	void LogException(const char* category, const std::exception& e) noexcept(false)
	{
		settings::getLogger()->Log(LogLevel::Fatal, category, e.what());
	}
#endif //ENABLE_THROW_ON_FATAL_LOG
}
