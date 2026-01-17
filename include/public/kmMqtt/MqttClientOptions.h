// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_MQTTCLIENTOPTIONS_H
#define INCLUDE_KMMQTT_MQTTCLIENTOPTIONS_H

#include "kmMqtt/Dispatchers/ImmediateDispatcher.h"
#include "kmMqtt/Interfaces/ICallbackDispatcher.h"
#include "kmMqtt/Dispatchers/DefaultDispatcher.h"

#include <memory>

namespace kmMqtt
{
	enum class TickMode : std::uint8_t
	{
		ASYNC, //Default option, tick() is called in self managed thread
		SYNC, //tick() must be called manually
	};

	/**
	 * @brief Options for configuring the MQTT client behavior.
	 */
	struct MqttClientOptions
	{
	public:
		MqttClientOptions(TickMode mode = TickMode::ASYNC)
		{
			tickMode(mode);
		}

		/**
		 * @brief Set the tick mode for the MQTT client.
		 * The tick mode determines how the client processes incoming messages and events.
		 * The ASYNC mode runs the tick function in a separate thread, while the SYNC mode requires
		 * the user to call the tick function manually in their application loop.
		 * 
		 * @param mode The desired tick mode (ASYNC or SYNC). Default is ASYNC.
		 * 
		 * @return Reference to the updated MqttClientOptions object.
		 */
		MqttClientOptions& tickMode(TickMode mode)
		{
			m_tickMode = mode;
			callbackDispatcher(m_callbackDispatcher); //Refresh
			return *this;
		}

		/**
		 * @brief Set a custom callback dispatcher for handling MQTT client callbacks. Works with both async and sync tick modes.
		 * If no dispatcher is provided, the default dispatcher will be used.
		 * 
		 * In ASYNC tick mode, if DefaultDispatcher is provided or no dispatcher is provided, ImmediateDispatcher will be used instead.
		 * 
		 * In SYNC tick mode, if DefaultDispatcher is provided or no dispatcher is provided, DefaultDispatcher will be used which is
		 * dependent on the user calling tick() to process callbacks.
		 * 
		 * @param callbackDispatcher The custom callback dispatcher to use.
		 * @return Reference to the updated MqttClientOptions object.
		 */
		MqttClientOptions& callbackDispatcher(const std::shared_ptr<ICallbackDispatcher>& callbackDispatcher)
		{
			if (getTickMode() == TickMode::ASYNC)
			{
				if (callbackDispatcher == nullptr)
				{
					m_callbackDispatcher = std::make_shared<ImmediateDispatcher>();
				}
				else if (std::dynamic_pointer_cast<DefaultDispatcher>(callbackDispatcher) != nullptr)
				{
					m_callbackDispatcher = std::make_shared<ImmediateDispatcher>();
				}
				else
				{
					m_callbackDispatcher = callbackDispatcher;
				}

				m_useInternalCallbackDeferrer = false;
			}
			else
			{
				m_callbackDispatcher = callbackDispatcher == nullptr ? std::make_shared<DefaultDispatcher>() : callbackDispatcher;
				m_useInternalCallbackDeferrer = std::dynamic_pointer_cast<DefaultDispatcher>(m_callbackDispatcher) != nullptr;
			}

			return *this;
		}

		/**
		 * @brief Get the current tick mode of the MQTT client.
		 * 
		 * @return The current TickMode.
		 */
		TickMode getTickMode() const
		{
			return m_tickMode;
		}

		/**
		 * @brief Get the current callback dispatcher used by the MQTT client.
		 * 
		 * @return Shared pointer to the ICallbackDispatcher instance.
		 */
		std::shared_ptr<ICallbackDispatcher> getCallbackDispatcher() const
		{
			return m_callbackDispatcher;
		}

		/**
		 * @brief Check if the internal callback deferrer is being used.
		 * 
		 * @return true if the internal callback deferrer is used, false otherwise.
		 */
		bool isUsingInternalCallbackDeferrer() const
		{
			return m_useInternalCallbackDeferrer;
		}

	private:
		TickMode m_tickMode{ TickMode::ASYNC };
		std::shared_ptr<ICallbackDispatcher> m_callbackDispatcher{ std::make_shared<DefaultDispatcher>()};
		bool m_useInternalCallbackDeferrer{ false };
	};
}

#endif //INCLUDE_KMMQTT_MQTTCLIENTOPTIONS_H