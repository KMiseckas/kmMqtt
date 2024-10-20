#ifndef INCLUDE_CLEANMQTT_MQTT_PARAMS_DISCONNECTARGS_H
#define INCLUDE_CLEANMQTT_MQTT_PARAMS_DISCONNECTARGS_H

#include <cleanMqtt/GlobalMacros.h>
#include <string>
#include <memory>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API DisconnectArgs
		{
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(DisconnectArgs)

			DisconnectArgs(bool requestWillPublish = false, bool clearSendQueue = true) noexcept
				: willPublish{ requestWillPublish },
				clearQueue{clearSendQueue}
			{
			}

			DisconnectArgs(DisconnectArgs&& other) noexcept
				: willPublish{other.willPublish},
				clearQueue{other.clearQueue},
				sessionExpiryInterval{other.sessionExpiryInterval},
				disconnectReasonText{std::move(other.disconnectReasonText)},
				userProperties{std::move(other.userProperties)}
			{
			}

			~DisconnectArgs() noexcept
			{
			}

			DisconnectArgs& operator=(DisconnectArgs&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				willPublish = other.willPublish;
				clearQueue = other.clearQueue;
				sessionExpiryInterval = other.sessionExpiryInterval;
				disconnectReasonText = std::move(other.disconnectReasonText);
				userProperties = std::move(other.userProperties);

				return *this;
			}

			bool willPublish{ false };
			bool clearQueue{ true };
			uint32_t sessionExpiryInterval{ 0U };
			std::string disconnectReasonText;

			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PARAMS_DISCONNECTARGS_H 
