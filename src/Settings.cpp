#include <cleanMqtt/Settings.h>

namespace cleanMqtt
{
	namespace settings
	{
		const interfaces::ILogger* const getLogger()
		{
			return m_logger;
		}

		void setLogger(interfaces::ILogger* logger, const bool deleteOld)
		{
			if (deleteOld && m_logger != nullptr)
			{
				delete(m_logger);
			}

			m_logger = logger;
		}
	}
}