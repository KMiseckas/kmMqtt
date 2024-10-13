#include <cleanMqtt/Logger/CleanLogger.h>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#endif

namespace cleanMqtt
{
	namespace logger
	{
		CleanLogger::CleanLogger() noexcept
		{
		}

		CleanLogger::~CleanLogger()
		{
		}

		void CleanLogger::Log(const LogLevel logLvl, const char* const msg) const noexcept
		{
			Log(logLvl, nullptr, msg);
		}

		void CleanLogger::Log(const LogLevel logLvl, const char* const category, const char* const msg) const noexcept
		{
			std::string logOutput;

			switch (logLvl)
			{
			case LogLevel::Trace:
				logOutput.append("[TRACE]");
				break;
			case LogLevel::Debug:
				logOutput.append("[DEBUG]");
				break;
			case LogLevel::Info:
				logOutput.append("[INFO]");
				break;
			case LogLevel::Warning:
				logOutput.append("[WARNING]");
				break;
			case LogLevel::Error:
				logOutput.append("[ERROR]");
				break;
			case LogLevel::Fatal:
				logOutput.append("[FATAL]");
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
}
