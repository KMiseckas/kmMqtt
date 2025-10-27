#ifndef INCLUDE_CLEANMQTT_MQTTCLIENTOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTTCLIENTOPTIONS_H

#include "cleanMqtt/Dispatchers/ImmediateDispatcher.h"
#include "cleanMqtt/Interfaces/ICallbackDispatcher.h"
#include "cleanMqtt/Dispatchers/DeferToTickEndDispatcher.h"

#include <memory>

namespace cleanMqtt
{
	/**
	 * @brief Options for configuring the MQTT client behavior.
	 */
	struct MqttClientOptions
	{
	public:
		MqttClientOptions() = default;

		/**
		 * @brief Set whether the MQTT client should operate in asynchronous tick mode.
		 * 
		 * @param tickAsync If true, the client will tick asynchronously in self managed thread.
		 * @param callbackDispatcher Optional custom callback dispatcher. If not provided, a default DeferToTickEndDispatcher will be used.
		 */
		void setTickAsync(bool tickAsync, const std::shared_ptr<ICallbackDispatcher>& callbackDispatcher = nullptr)
		{
			m_tickAsync = tickAsync;
			setCallbackDispatcher(callbackDispatcher);
		}

		/**
		 * @brief Set a custom callback dispatcher for handling MQTT client callbacks. Works with both async and sync tick modes.
		 * 
		 * @param callbackDispatcher The custom callback dispatcher to use. If nullptr, a default DeferToTickEndDispatcher will be used.
		 */
		void setCallbackDispatcher(const std::shared_ptr<ICallbackDispatcher>& callbackDispatcher)
		{
			m_callbackDispatcher = callbackDispatcher == nullptr ? std::make_shared<DeferToTickEndDispatcher>() : callbackDispatcher;

			m_useInternalCallbackDeferrer = typeid(*m_callbackDispatcher) == typeid(DeferToTickEndDispatcher);
		}

		bool isTickAsync() const
		{
			return m_tickAsync;
		}

		std::shared_ptr<ICallbackDispatcher> getCallbackDispatcher() const
		{
			return m_callbackDispatcher;
		}

		bool isUsingInternalCallbackDeferrer() const
		{
			return m_useInternalCallbackDeferrer;
		}

	private:
		bool m_tickAsync{ false };
		std::shared_ptr<ICallbackDispatcher> m_callbackDispatcher{ std::make_shared<DeferToTickEndDispatcher>()};
		bool m_useInternalCallbackDeferrer{ false };
	};
}

#endif //INCLUDE_CLEANMQTT_MQTTCLIENTOPTIONS_H
