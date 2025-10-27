#ifndef INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H
#define INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H

#include "cleanMqtt/GlobalMacros.h"
#include <cleanMqtt/Interfaces/IWebSocket.h>
#include <cleanMqtt/Interfaces/ILogger.h>
#include <cleanMqtt/Config.h>
#include <memory>
#include <string>

namespace cleanMqtt
{
	class PUBLIC_API IMqttEnvironment
	{
	public:
		virtual ~IMqttEnvironment() = default;

		virtual Config createConfig() const noexcept = 0;

		virtual std::unique_ptr<IWebSocket> createWebSocket() const noexcept = 0;
	};
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H
