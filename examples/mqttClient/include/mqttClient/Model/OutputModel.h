// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef MQTTCLIENT_MODEL_OUTPUTMODEL_H
#define MQTTCLIENT_MODEL_OUTPUTMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <kmMqtt/MqttClient.h>
#include <kmMqtt/Interfaces/ILogger.h>
#include <kmMqtt/STL/KmThread.h>

#include <kmMqtt/STL/KmString.h>
#include <kmMqtt/STL/KmVector.h>
#include <fstream>

struct OutputMsgMetadata
{
	kmMqtt::LogLevel logLevel;
	kmMqtt::kmStd::string logEntry{ "" };
};

class OutputModel : public ViewModel
{
public:
	OutputModel() noexcept;
	~OutputModel() override;

	void setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept;
	void AddOutput(kmMqtt::LogLevel logLevel, const kmMqtt::kmStd::string category, const kmMqtt::kmStd::string msg);
	const OutputMsgMetadata* const GetAllLogs() const noexcept;
	void clearLogs() noexcept;
	
	void enableFileLogging(const kmMqtt::kmStd::string& filepath);
	void disableFileLogging();
	inline bool isFileLoggingEnabled() const noexcept { return m_fileLoggingEnabled; }

	std::size_t getLogCount() const noexcept;

	struct UIData
	{
		bool showClearConfirmation{ false };
		bool autoScroll{ true };
	} uiData;

private:
	void writeToFile(const kmMqtt::kmStd::string& logEntry);

	OutputMsgMetadata m_logs[1024];
	OutputMsgMetadata* nextLogEntry{ m_logs };
	std::size_t logCount{ 0 };
	std::ofstream m_logFile;
	kmMqtt::kmStd::mutex m_logMutex;
	bool m_fileLoggingEnabled = false;

	kmMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
};


#endif // MQTTCLIENT_MODEL_OUTPUTMODEL_H
