#ifndef INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H
#define INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Config.h"
#include "cleanMqtt/Interfaces/ILogger.h"
#include "cleanMqtt/Interfaces/IWebSocket.h"

#include <memory>
#include <string>

namespace cleanMqtt
{
	/**
	 * @brief Interface for creating MQTT environment-specific components.
	 * 
	 * This interface defines the factory methods for creating essential MQTT client
	 * components.
	 * Implementations of this interface provide the environment-specific behavior
	 * for these components.
	 */
	class PUBLIC_API IMqttEnvironment
	{
	public:
		/**
		 * @brief Virtual destructor.
		 */
		virtual ~IMqttEnvironment() = default;

		/**
		 * @brief Creates a configuration object for the MQTT client.
		 * 
		 * @return Config object containing the MQTT client configuration settings.
		 */
		virtual Config createConfig() const noexcept = 0;

		/**
		 * @brief Creates a web socket instance for MQTT communication.
		 * 
		 * @return A shared pointer to an IWebSocket implementation for network communication.
		 */
		virtual std::shared_ptr<IWebSocket> createWebSocket() const noexcept = 0;
	};
}

#endif //INCLUDE_CLEANMQTT_INTERFACES_IMQTTENVIRONMENT_H
