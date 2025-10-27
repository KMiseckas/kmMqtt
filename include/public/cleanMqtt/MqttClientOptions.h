#ifndef INCLUDE_CLEANMQTT_MQTTCLIENTOPTIONS_H
#define INCLUDE_CLEANMQTT_MQTTCLIENTOPTIONS_H

#include "cleanMqtt/Dispatchers/ImmediateDispatcher.h"
#include "cleanMqtt/Interfaces/ICallbackDispatcher.h"
#include "cleanMqtt/Dispatchers/DeferToTickEndDispatcher.h"

#include <memory>

namespace cleanMqtt
{
	struct MqttClientOptions
	{
	public:
		MqttClientOptions() = default;

		void setTickAsync(bool tickAsync, const std::shared_ptr<ICallbackDispatcher>& callbackDispatcher = nullptr)
		{
			m_tickAsync = tickAsync;
			setCallbackDispatcher(callbackDispatcher);
		}

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
