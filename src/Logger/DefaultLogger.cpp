#include <cleanMqtt/Logger/DefaultLogger.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#endif

namespace cleanMqtt
{
	DefaultLogger::DefaultLogger() noexcept
	{
	}

	DefaultLogger::~DefaultLogger()
	{
	}

	void DefaultLogger::Log(const LogLevel logLvl, const char* const msg) const noexcept
	{
		Log(logLvl, nullptr, msg);
	}

	void DefaultLogger::Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept
	{
		std::string logOutput{ "[CleanMqtt]" };

		switch (logLvl)
		{
		case LogLevel::Trace:
			logOutput.append("[Trace]");
			break;
		case LogLevel::Debug:
			logOutput.append("[Debug]");
			break;
		case LogLevel::Info:
			logOutput.append("[Info]");
			break;
		case LogLevel::Warning:
			logOutput.append("[Warning]");
			break;
		case LogLevel::Error:
			logOutput.append("[Error]");
			break;
		case LogLevel::Fatal:
			logOutput.append("[Fatal]");
			break;
		}

		if (category != nullptr)
		{
			logOutput.append("[").append(category).append("]");
		}

		logOutput.append(" ").append(msg == nullptr ? "" : msg).append("\n");


#ifdef _WIN32
		OutputDebugStringA(logOutput.c_str());
#else
		fprintf(stderr, "%s", logOutput.c_str());
#endif
	}
}
