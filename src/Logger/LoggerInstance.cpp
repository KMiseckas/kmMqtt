#include <kmMqtt/Logger/LoggerInstance.h>

namespace kmMqtt
{
	ILogger* m_logger{ nullptr };

	ILogger* getLogger()
	{
		return m_logger;
	}

	void setLogger(ILogger* logger, const bool deleteOld)
	{
		if (deleteOld && m_logger != nullptr)
		{
			delete m_logger;
		}

		m_logger = logger;
	}
}