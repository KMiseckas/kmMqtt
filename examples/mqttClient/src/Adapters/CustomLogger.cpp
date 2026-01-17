// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <mqttClient/Adapters/CustomLogger.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#endif

CustomLogger::CustomLogger(std::weak_ptr<OutputModel> outputModel) noexcept
	: m_outputModel{ std::move(outputModel) }
{
}

CustomLogger::~CustomLogger()
{
}

void CustomLogger::Log(const kmMqtt::LogLevel logLvl, const char* const msg) const noexcept
{
	Log(logLvl, nullptr, msg);
}

void CustomLogger::Log(const kmMqtt::LogLevel logLvl, const char* const category, const char* const msg) const noexcept
{
	std::string logOutput{ "[kmMqtt]" };

	switch (logLvl)
	{
	case kmMqtt::LogLevel::Trace:
		logOutput.append("[Trace]");
		break;
	case kmMqtt::LogLevel::Debug:
		logOutput.append("[Debug]");
		break;
	case kmMqtt::LogLevel::Info:
		logOutput.append("[Info]");
		break;
	case kmMqtt::LogLevel::Warning:
		logOutput.append("[Warning]");
		break;
	case kmMqtt::LogLevel::Error:
		logOutput.append("[Error]");
		break;
	case kmMqtt::LogLevel::Fatal:
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

	if (auto model = m_outputModel.lock())
	{
		model->AddOutput(logLvl, category == nullptr ? "" : category, msg == nullptr ? "" : msg);
	}
}
