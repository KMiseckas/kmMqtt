#ifndef MQTTCLIENT_MODEL_OUTPUTMODEL_H
#define MQTTCLIENT_MODEL_OUTPUTMODEL_H

#include "mqttClient/Model/ViewModel.h"
#include <cleanMqtt/MqttClient.h>
#include <cleanMqtt/Interfaces/ILogger.h>

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

struct OutputMsgMetadata
{
	cleanMqtt::LogLevel logLevel;
	std::string logEntry{ "" };
};

class OutputModel : public ViewModel
{
public:
	OutputModel() noexcept;
	~OutputModel() override;

	void setMqttClient(cleanMqtt::mqtt::MqttClient* client) noexcept;
	void AddOutput(cleanMqtt::LogLevel logLevel, const std::string category, const std::string msg);
	const OutputMsgMetadata* const GetAllLogs() const noexcept;
	void clearLogs() noexcept;
	
	void enableFileLogging(const std::string& filepath);
	void disableFileLogging();

	std::size_t getLogCount() const noexcept;

	struct UIData
	{
		bool showClearConfirmation{ false };
		bool autoScroll{ true };
	} uiData;

private:
	void writeToFile(const std::string& logEntry);

	OutputMsgMetadata m_logs[1024];
	OutputMsgMetadata* nextLogEntry{ m_logs };
	std::size_t logCount{ 0 };
	std::ofstream m_logFile;
	std::mutex m_logMutex;
	bool m_fileLoggingEnabled = false;

	cleanMqtt::mqtt::MqttClient* m_mqttClient{ nullptr };
};


#endif // MQTTCLIENT_MODEL_OUTPUTMODEL_H
