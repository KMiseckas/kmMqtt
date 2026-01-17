// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef MQTTCLIENT_ADAPTERS_CUSTOMLOGGER_H
#define MQTTCLIENT_ADAPTERS_CUSTOMLOGGER_H

#include <kmMqtt/Logger/DefaultLogger.h>
#include <kmMqtt/Interfaces/ILogger.h>
#include <mqttClient/Model/OutputModel.h>

#include <memory>

class CustomLogger : public kmMqtt::DefaultLogger
{
public:
	CustomLogger(std::weak_ptr<OutputModel> outputModel) noexcept;

	~CustomLogger() override;

	void Log(const kmMqtt::LogLevel logLvl, const char* const msg) const noexcept override;
	void Log(const kmMqtt::LogLevel logLvl, const char* const category, const char* const msg) const noexcept override;

private:

	std::weak_ptr<OutputModel> m_outputModel;
};

#endif // MQTTCLIENT_ADAPTERS_CUSTOMLOGGER_H
