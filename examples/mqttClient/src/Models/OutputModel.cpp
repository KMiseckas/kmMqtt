#include "mqttClient/Model/OutputModel.h"

#include <kmMqtt/Interfaces/ILogger.h>
#include <kmMqtt/MqttClient.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>

OutputModel::OutputModel() noexcept
{
}

OutputModel::~OutputModel()
{
	disableFileLogging();
}

void OutputModel::setMqttClient(kmMqtt::mqtt::MqttClient* client) noexcept
{
	m_mqttClient = client;
}

void OutputModel::AddOutput(kmMqtt::LogLevel logLevel, const std::string category, const std::string msg)
{
	std::lock_guard<std::mutex> lock(m_logMutex);

	std::string logEntry;

	//Add timestamp to beginning.
	auto now{ std::chrono::system_clock::now() };
	auto time_t{ std::chrono::system_clock::to_time_t(now) };

	std::stringstream ss;
	std::tm tm_buf{};
#ifdef _WIN32
	localtime_s(&tm_buf, &time_t);
	ss << "[" << std::put_time(&tm_buf, "%H:%M:%S") << "] ";
#else
	localtime_r(&time_t, &tm_buf);
	ss << "[" << std::put_time(&tm_buf, "%H:%M:%S") << "] ";
#endif
	logEntry = ss.str();

	switch (logLevel)
	{
	case kmMqtt::LogLevel::Trace:
		logEntry.append("[Trace]");
		break;
	case kmMqtt::LogLevel::Debug:
		logEntry.append("[Debug]");
		break;
	case kmMqtt::LogLevel::Info:
		logEntry.append("[Info]");
		break;
	case kmMqtt::LogLevel::Warning:
		logEntry.append("[Warning]");
		break;
	case kmMqtt::LogLevel::Error:
		logEntry.append("[Error]");
		break;
	case kmMqtt::LogLevel::Fatal:
		logEntry.append("[Fatal]");
		break;
	}

	if (!category.empty())
	{
		logEntry.append("[").append(category).append("]");
	}

	logEntry.append(" ").append(msg);

	if (nextLogEntry >= m_logs + sizeof(m_logs) / sizeof(m_logs[0]))
	{
		//Delete entries at beginning and shift remaining down
		static constexpr size_t deleteCount{ 15 };
		static constexpr size_t totalSize{ sizeof(m_logs) / sizeof(m_logs[0]) };

		for (size_t i = 0; i < totalSize - deleteCount; ++i)
		{
			m_logs[i] = m_logs[i + deleteCount];
		}

		nextLogEntry = m_logs + (totalSize - deleteCount);
		logCount = totalSize - deleteCount;
	}

	*nextLogEntry = OutputMsgMetadata{ logLevel, logEntry };
	nextLogEntry++;
	logCount++;

	//Write to file if enabled
	if (m_fileLoggingEnabled)
	{
		writeToFile(logEntry);
	}
}

const OutputMsgMetadata* const OutputModel::GetAllLogs() const noexcept
{
	return m_logs;
}

void OutputModel::clearLogs() noexcept
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	
	for (size_t i = 0; i < sizeof(m_logs) / sizeof(m_logs[0]); ++i)
	{
		m_logs[i] = OutputMsgMetadata{};
	}

	logCount = 0;
	nextLogEntry = m_logs;
}

void OutputModel::enableFileLogging(const std::string& filepath)
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	
	if (m_fileLoggingEnabled)
	{
		m_logFile.close();
	}

	m_logFile.open(filepath, std::ios::out | std::ios::app);
	if (m_logFile.is_open())
	{
		m_fileLoggingEnabled = true;
		
		auto now{ std::chrono::system_clock::now() };
		auto time_t{ std::chrono::system_clock::to_time_t(now) };
		std::tm tm_buf{};
#ifdef _WIN32
		localtime_s(&tm_buf, &time_t);
#else
		localtime_r(&time_t, &tm_buf);
#endif
		m_logFile << "\n=== MqttClient - Log Session Started: " 
				  << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") 
				  << " ===\n";
		m_logFile.flush();
	}
}

void OutputModel::disableFileLogging()
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	
	if (m_fileLoggingEnabled && m_logFile.is_open())
	{
		m_logFile.close();
		m_fileLoggingEnabled = false;
	}
}

void OutputModel::writeToFile(const std::string& logEntry)
{
	if (m_logFile.is_open())
	{
		m_logFile << logEntry << std::endl;
		m_logFile.flush();
	}
}

std::size_t OutputModel::getLogCount() const noexcept
{
	return logCount;
}