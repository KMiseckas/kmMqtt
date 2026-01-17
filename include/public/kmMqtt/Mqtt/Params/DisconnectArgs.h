#ifndef INCLUDE_KMMQTT_MQTT_PARAMS_DISCONNECTARGS_H
#define INCLUDE_KMMQTT_MQTT_PARAMS_DISCONNECTARGS_H

#include <kmMqtt/GlobalMacros.h>
#include <kmMqtt/Mqtt/Enums/PayloadFormatIndicator.h>
#include <string>
#include <memory>
#include <map>

namespace kmMqtt
{
	namespace mqtt
	{
		struct PUBLIC_API DisconnectArgs
		{
			DisconnectArgs(bool gracefulDisconnect = true, bool requestWillPublish = false, bool clearSendQueue = true) noexcept
				: gracefulDisconnect{ gracefulDisconnect },
				willPublish{ requestWillPublish },
				clearQueue{clearSendQueue}
			{
			}

			DisconnectArgs(DisconnectArgs&& other) noexcept
				: gracefulDisconnect{other.gracefulDisconnect},
				willPublish{ other.willPublish },
				clearQueue{other.clearQueue},
				sessionExpiryInterval{other.sessionExpiryInterval},
				disconnectReasonText{std::move(other.disconnectReasonText)},
				userProperties{std::move(other.userProperties)}
			{
			}

			DisconnectArgs(const DisconnectArgs& other)
			{
				gracefulDisconnect = other.gracefulDisconnect;
				willPublish = other.willPublish;
				clearQueue = other.clearQueue;
				sessionExpiryInterval = other.sessionExpiryInterval;
				disconnectReasonText = other.disconnectReasonText;
				userProperties = other.userProperties;
			}

			~DisconnectArgs() noexcept
			{
			}

			DisconnectArgs& operator=(const DisconnectArgs& other)
			{
				if (this == &other)
				{
					return *this;
				}

				gracefulDisconnect = other.gracefulDisconnect;
				willPublish = other.willPublish;
				clearQueue = other.clearQueue;
				sessionExpiryInterval = other.sessionExpiryInterval;
				disconnectReasonText = other.disconnectReasonText;
				userProperties = other.userProperties;

				return *this;
			}

			DisconnectArgs& operator=(DisconnectArgs&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				gracefulDisconnect = other.gracefulDisconnect;
				willPublish = other.willPublish;
				clearQueue = other.clearQueue;
				sessionExpiryInterval = other.sessionExpiryInterval;
				disconnectReasonText = std::move(other.disconnectReasonText);
				userProperties = std::move(other.userProperties);

				return *this;
			}

			bool gracefulDisconnect{ true };
			bool willPublish{ false };
			bool clearQueue{ true };
			uint32_t sessionExpiryInterval{ 0U };
			std::string disconnectReasonText;

			std::map<std::string, std::string> userProperties;
		};
	}
}

#endif //INCLUDE_KMMQTT_MQTT_PARAMS_DISCONNECTARGS_H 
