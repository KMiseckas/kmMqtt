#include <kmMqtt/Logger/Log.h>
#include <kmMqtt/Logger/LoggerInstance.h>
#include <cstdarg>

namespace kmMqtt
{

#define TRACE 0
#define INFO 1
#define DEBUG 2
#define WARNING 3
#define ERROR 4

#ifndef ENABLE_LOGS
#define LOG_LEVEL 999
#endif //ENABLE_LOGS

#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE 2048
#endif

#if _WIN32 || _WIN64
#define VSNPRINTF(buffer, size, fmt, args) _vsnprintf_s(buffer, size, _TRUNCATE, fmt, args)
#else
#include <cstdio>
#define VSNPRINTF(buffer, size, fmt, args) std::vsnprintf(buffer, size, fmt, args)
#endif

namespace
{
#define FORMAT_MSG(msg, lastParam)\
	char buffer[LOG_BUFFER_SIZE];\
	va_list args;\
	va_start(args, lastParam);\
	VSNPRINTF(buffer, sizeof(buffer), msg, args);\
	va_end(args);\

#define LOG(logLevel, msg, lastParam)\
	FORMAT_MSG(msg, lastParam)\
	getLogger()->Log(logLevel, buffer);\

#define LOG_CATEGORY(logLevel, category, msg, lastParam)\
	FORMAT_MSG(msg, lastParam)\
	getLogger()->Log(logLevel,category, buffer);\

}

#if LOG_LEVEL <= TRACE
	void LogTrace(const char* msg, ...) noexcept
	{
		LOG(LogLevel::Trace, msg, msg);
	}

	void LogTrace(const char* category, const char* msg, ...) noexcept
	{
		LOG_CATEGORY(LogLevel::Trace, category, msg, msg);
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
		LOG(LogLevel::Info, msg, msg);
	}

	void LogInfo(const char* category, const char* msg, ...) noexcept
	{
		LOG_CATEGORY(LogLevel::Info, category, msg, msg);
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
		LOG(LogLevel::Debug, msg, msg);
	}

	void LogDebug(const char* category, const char* msg, ...) noexcept
	{
		LOG_CATEGORY(LogLevel::Debug, category, msg, msg);
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
		LOG(LogLevel::Warning, msg, msg);
	}

	void LogWarning(const char* category, const char* msg, ...) noexcept
	{
		LOG_CATEGORY(LogLevel::Warning, category, msg, msg);
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
		LOG(LogLevel::Error, msg, msg);
	}

	void LogError(const char* category, const char* msg, ...) noexcept
	{
		LOG_CATEGORY(LogLevel::Error, category, msg, msg);
	}
#else
	void LogError(const char* /*msg*/, ...) noexcept
	{
	}

	void LogError(const char* /*category*/, const char* /*msg*/, ...) noexcept
	{
	}
#endif //LOG_LEVEL <= ERROR

	void LogException(const char* category, const std::exception& e) noexcept(false)
	{
		getLogger()->Log(LogLevel::Fatal, category, e.what());
		throw e;
	}
}
